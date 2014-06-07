<?php

/**
 * @file
 * Contains Drupal\visitors\Controller\Report\Hits.
 */

namespace Drupal\visitors\Controller\Report;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Datetime\Date;
use Drupal\Core\Form\FormBuilderInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class Hits extends ControllerBase {
  /**
   * The date service.
   *
   * @var \Drupal\Core\Datetime\Date
   */
  protected $date;

  /**
   * The form builder service.
   *
   * @var \Drupal\Core\Form\FormBuilderInterface
   */
  protected $formBuilder;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('date'),
      $container->get('form_builder')
    );
  }

  /**
   * Constructs a Hits object.
   *
   * @param \Drupal\Core\Datetime\Date $date
   *   The date service.
   *
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder service.
   */
  public function __construct(Date $date, FormBuilderInterface $form_builder) {
    $this->date        = $date;
    $this->formBuilder = $form_builder;
  }

  /**
   * Returns a hits page.
   *
   * @return array
   *   A render array representing the hits page content.
   */
  public function display($host) {
    $form = $this->formBuilder->getForm('Drupal\visitors\Form\DateFilter');
    $header = $this->_getHeader();

    return array(
      '#title' => check_plain(t('Hits from') . ' ' . $host),
      'visitors_date_filter_form' => $form,
      'visitors_table' => array(
        '#theme'  => 'table',
        '#header' => $header,
        '#rows'   => $this->_getData($header, $host),
      ),
      'visitors_pager' => array('#theme' => 'pager')
    );
  }

  /**
   * Returns a table header configuration.
   *
   * @return array
   *   A render array representing the table header info.
   */
  protected function _getHeader() {
    return array(
      '#' => array(
        'data'      => t('#'),
      ),
      'visitors_id' => array(
        'data'      => t('ID'),
        'field'     => 'visitors_id',
        'specifier' => 'visitors_id',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
        'sort'      => 'desc',
      ),
      'visitors_date_time' => array(
        'data'      => t('Date'),
        'field'     => 'visitors_date_time',
        'specifier' => 'visitors_date_time',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
      ),
      'visitors_url' => array(
        'data'      => t('URL'),
        'field'     => 'visitors_url',
        'specifier' => 'visitors_url',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
      ),
      'u.name' => array(
        'data'      => t('User'),
        'field'     => 'u.name',
        'specifier' => 'u.name',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
      ),
      '' => array(
        'data'      => t('Operations'),
      ),
    );
  }

  /**
   * Returns a table content.
   *
   * @param array $header
   *   Table header configuration.
   *
   * @return array
   *   Array representing the table content.
   */
  protected function _getData($header, $host) {
    if (@inet_pton($host) === FALSE) {
      return;
    }

    $items_per_page = \Drupal::config('visitors.config')->get('items_per_page');

    $query = db_select('visitors', 'v')
      ->extend('Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('Drupal\Core\Database\Query\TableSortExtender');

    $query->leftJoin('users', 'u', 'u.uid=v.visitors_uid');
    $query->fields(
      'v',
      array(
        'visitors_id',
        'visitors_ip',
        'visitors_uid',
        'visitors_date_time',
        'visitors_title',
        'visitors_path',
        'visitors_url'
      )
    );
    $query->fields('u', array('name', 'uid'));
    $query->condition('v.visitors_ip', sprintf('%u', ip2long($host)), '=');
    visitors_date_filter_sql_condition($query);
    $query->orderByHeader($header);
    $query->limit($items_per_page);

    $count_query = db_select('visitors', 'v');
    $count_query->addExpression('COUNT(*)');
    $count_query->condition('visitors_ip', sprintf('%u', ip2long($host)));
    visitors_date_filter_sql_condition($count_query);
    $query->setCountQuery($count_query);
    $results = $query->execute();

    $count = $count_query->execute()->fetchField();
    if ($count == 0) {
      return;
    }
    $rows = array();

    $page = isset($_GET['page']) ? $_GET['page'] : '';
    $i = 0 + $page * $items_per_page;

    foreach ($results as $data) {
      $user = user_load($data->visitors_uid);
      $username = array('#theme' => 'username', '#account' => $user);
      $rows[] = array(
        ++$i,
        $data->visitors_id,
          $this->date->format($data->visitors_date_time, 'short'),
              check_plain($data->visitors_title) . '<br/>' .
          l($data->visitors_path, $data->visitors_url),
        drupal_render($username),
        l(t('details'), 'visitors/hits/' . $data->visitors_id)
      );
    }

    return $rows;
  }
}

