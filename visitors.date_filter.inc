<?php

function visitors_date_filter() {
  return drupal_get_form('visitors_date_filter_form');
}

function visitors_set_session_date_range() {
  if (!isset($_SESSION['visitors_from'])) {
    $_SESSION['visitors_from'] = array(
      'day'   => 1,
      'month' => date('n'),
      'year'  => date('Y'),
    );
  }

  if (!isset($_SESSION['visitors_to'])) {
    $_SESSION['visitors_to'] = array(
      'day'   => date('j'),
      'month' => date('n'),
      'year'  => date('Y'),
    );
  }
}

function visitors_date_filter_form($form_state) {
  visitors_set_session_date_range();

  $form = array();

  $form['visitors_date_filter'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Date filter'),
    '#collapsible'   => FALSE,
    '#collapsed'     => FALSE,
    '#description'   => t('Choose date range')
  );

  $form['visitors_date_filter']['from'] = array(
    '#type'          => 'date',
    '#title'         => t('From'),
    '#default_value' => $_SESSION['visitors_from']
  );

  $form['visitors_date_filter']['to'] = array(
    '#type'          => 'date',
    '#title'         => t('To'),
    '#default_value' => $_SESSION['visitors_to']
  );

  $form['visitors_date_filter']['submit'] = array(
    '#type'          => 'submit',
    '#value'         => t('Submit'),
  );

  return $form;
}

function visitors_date_filter_form_validate($form, &$form_state) {
  $from = mktime(0, 0, 0,
    $form_state['values']['from']['month'],
    $form_state['values']['from']['day'],
    $form_state['values']['from']['year']
  );

  $to  = mktime(23, 59, 59,
    $form_state['values']['to']['month'],
    $form_state['values']['to']['day'],
    $form_state['values']['to']['year']
  );

  if ($from > $to) {
    form_set_error('from', t('Incorrect date range.'));
  }
}

function visitors_date_filter_form_submit($form, &$form_state) {
  $_SESSION['visitors_from'] = $form_state['values']['from'];
  $_SESSION['visitors_to']   = $form_state['values']['to'];
}

function visitors_get_from_timestamp() {
  return mktime(0, 0, 0,
                $_SESSION['visitors_from']['month'],
                $_SESSION['visitors_from']['day'],
                $_SESSION['visitors_from']['year']
  );
}

function visitors_get_to_timestamp() {
  return mktime(23, 59, 59,
                $_SESSION['visitors_to']['month'],
                $_SESSION['visitors_to']['day'],
                $_SESSION['visitors_to']['year']
  );
}

function visitors_date_filter_sql_condition() {
  visitors_set_session_date_range();

  $from = visitors_get_from_timestamp();
  $to   = visitors_get_to_timestamp();

  return sprintf('visitors_date_time BETWEEN %d AND %d', $from, $to);
}
