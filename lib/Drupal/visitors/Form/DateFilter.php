<?php

namespace Drupal\visitors\Form;

use Drupal\Core\Datetime\DrupalDateTime;
use Drupal\Core\Form\FormBase;
use Drupal\Component\Utility\NestedArray;
use Drupal\datetime\DateHelper;

class DateFilter extends FormBase {
  protected $_order = array('month', 'day', 'year');

  protected function _getOrder() {
    return $this->_order;
  }

  public function getFormID() {
    return 'date_filter_form';
  }

  /**
   * Set to session info default values for visitors date filter.
   */
  protected function _setSessionDateRange() {
    if (!isset($_SESSION['visitors_from'])) {
      $_SESSION['visitors_from'] = array(
        'month' => date('n'),
        'day'   => 1,
        'year'  => date('Y'),
      );
    }

    if (!isset($_SESSION['visitors_to'])) {
      $_SESSION['visitors_to'] = array(
        'month' => date('n'),
        'day'   => date('j'),
        'year'  => date('Y'),
      );
    }
  }

  public function buildForm(array $form, array &$form_state) {
    $this->_setSessionDateRange();

    $from = DrupalDateTime::createFromArray($_SESSION['visitors_from']);
    $to   = DrupalDateTime::createFromArray($_SESSION['visitors_to']);

    $form = array();

    $form['visitors_date_filter'] = array(
      '#type'             => 'fieldset',
      '#title'            => t('Date filter'),
      '#collapsible'      => TRUE,
      '#collapsed'        => TRUE,
      '#description'      => t('Choose date range')
    );

    $form['visitors_date_filter']['from'] = array(
      '#type'             => 'datelist',
      '#title'            => t('From'),
      '#date_part_order'  => $this->_getOrder(),
      '#default_value'    => $from,
      '#date_timezone'    => drupal_get_user_timezone(),
      '#element_validate' => array(array($this, 'datelistValidate')),
      '#process'          => array(array($this, 'formProcessDatelist')),
      '#value_callback'   => array($this, 'datelistValueCallback'),
    );

    $form['visitors_date_filter']['to'] = array(
      '#type'             => 'datelist',
      '#title'            => t('To'),
      '#date_part_order'  => $this->_getOrder(),
      '#default_value'    => $to,
      '#date_timezone'    => drupal_get_user_timezone(),
      '#element_validate' => array(array($this, 'datelistValidate')),
      '#process'          => array(array($this, 'formProcessDatelist')),
      '#value_callback'   => array($this, 'datelistValueCallback'),
    );

    $form['visitors_date_filter']['submit'] = array(
      '#type'             => 'submit',
      '#value'            => t('Save'),
    );

    return $form;
  }

  public function validateForm(array &$form, array &$form_state) {
    $from          = $form_state['input']['from'];
    $to            = $form_state['input']['to'];

    $from['month'] = (int) $from['month'];
    $from['day']   = (int) $from['day'];
    $from['year']  = (int) $from['year'];

    $to['month']   = (int) $to['month'];
    $to['day']     = (int) $to['day'];
    $to['year']    = (int) $to['year'];

    $error_message = t('The specified date is invalid.');

    if (!checkdate($from['month'], $from['day'], $from['year'])) {
      return $this->setFormError('from', $form_state, $error_message);
    }

    if (!checkdate($to['month'], $to['day'], $to['year'])) {
      return $this->setFormError('to', $form_state, $error_message);
    }

    $from = mktime(0, 0, 0, $from['month'], $from['day'], $from['year']);
    $to = mktime(23, 59, 59, $to['month'], $to['day'], $to['year']);

    if ((int) $from <= 0) {
      return $this->setFormError('from', $form_state, $error_message);
    }

    if ((int) $to <= 0) {
      return $this->setFormError('to', $form_state, $error_message);
    }

    if ($from > $to) {
      return $this->setFormError('from', $form_state, $error_message);
    }
  }

  public function submitForm(array &$form, array &$form_state) {
    $from = $form_state['values']['from'];
    $to   = $form_state['values']['to'];

    $_SESSION['visitors_from'] = array(
      'month' => $from->format('n'),
      'day'   => $from->format('j'),
      'year'  => $from->format('Y'),
    );

    $_SESSION['visitors_to'] = array(
      'month' => $to->format('n'),
      'day'   => $to->format('j'),
      'year'  => $to->format('Y'),
    );
  }

  /**
   * Validates the date type to adjust 12 hour time and prevent invalid
   * dates (e.g., February 30, 2006).
   *
   * If the date is valid, the date is set in the form as a string
   * using the format designated in __toString().
   */
  function datelistValidate($element, &$form_state) {
    $input_exists = FALSE;

    $input = NestedArray::getValue(
      $form_state['values'],
      $element['#parents'],
      $input_exists
    );

    if (!$input_exists) {
      return;
    }

    // If there's empty input, set an error.
    if (
      empty($input['year']) ||
      empty($input['month']) ||
      empty($input['day'])
    ) {
      form_error($element, $form_state, t('The %field date is required.'));
      return;
    }

    if (!checkdate($input['month'], $input['day'], $input['year'])) {
      form_error($element, $form_state, t('The specified date is invalid.'));
      return;
    }

    $date = DrupalDateTime::createFromArray($input);

    if ($date instanceOf DrupalDateTime && !$date->hasErrors()) {
      form_set_value($element, $date, $form_state);
    } else {
      form_error($element, $form_state, t('The %field date is invalid.'));
    }
  }

  function datelistValueCallback($element, $input = FALSE, &$form_state = array()) {
    $parts  = $this->_getOrder();
    $return = array_fill_keys($parts, '');

    foreach ($parts as $part) {
      $return[$part] = $input[$part];
    }

    return $return;
  }

  function formProcessDatelist($element, &$form_state) {
    if (
      empty($element['#value']['month']) ||
      empty($element['#value']['day']) ||
      empty($element['#value']['year'])
    ) {
      $element['#value'] = array(
        'month' => $element['#default_value']->format('n'),
        'day'   => $element['#default_value']->format('j'),
        'year'  => $element['#default_value']->format('Y')
      );
    }

    $element['#tree'] = TRUE;

    // Output multi-selector for date.
    foreach ($this->_getOrder() as $part) {
      switch ($part) {
        case 'month':
          $options = DateHelper::monthNamesAbbr(TRUE);
          $title = t('Month');
          break;

        case 'day':
          $options = DateHelper::days(TRUE);
          $title = t('Day');
          break;

        case 'year':
          $options = DateHelper::years(2012, 2014, TRUE);
          $title = t('Year');
          break;
      }

      $element['#attributes']['title'] = $title;

      $element[$part] = array(
        '#attributes'    => $element['#attributes'],
        '#options'       => $options,
        '#required'      => $element['#required'],
        '#title'         => $title,
        '#title_display' => 'invisible',
        '#type'          => 'select',
        '#value'         => (int) $element['#value'][$part],
      );
    }

    return $element;
  }
}

