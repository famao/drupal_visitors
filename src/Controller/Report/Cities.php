<?php

/**
 * @file
 * Contains Drupal\visitors\Controller\Report\Cities.
 */

namespace Drupal\visitors\Controller\Report;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Datetime\Date;
use Drupal\Core\Form\FormBuilderInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class Cities extends ControllerBase {
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
   * Constructs a Cities object.
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
   * Returns a cities page.
   *
   * @return array
   *   A render array representing the cities page content.
   */
  public function display($country) {
    $form = $this->formBuilder->getForm('Drupal\visitors\Form\DateFilter');
    $header = $this->_getHeader();

    return array(
      '#title' => t('Visitors from') . ' ' . t($country),
      'visitors_date_filter_form' => $form,
      'visitors_table' => array(
        '#theme'  => 'table',
        '#header' => $header,
        '#rows'   => $this->_getData($header, $country),
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
      'visitors_city' => array(
        'data'      => t('City'),
        'field'     => 'visitors_city',
        'specifier' => 'visitors_city',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
      ),
      'count' => array(
        'data'      => t('Count'),
        'field'     => 'count',
        'specifier' => 'count',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
        'sort'      => 'desc',
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
  protected function _getData($header, $country) {
    $items_per_page = \Drupal::config('visitors.config')->get('items_per_page');
    $original_country =  ($country == '(none)') ? '' : $country;
  
    $query = db_select('visitors', 'v')
      ->extend('Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('Drupal\Core\Database\Query\TableSortExtender');
  
    $query->addExpression('COUNT(visitors_city)', 'count');
    $query->fields('v', array('visitors_city'));
    $query->condition('v.visitors_country_name', $original_country);
    visitors_date_filter_sql_condition($query);
    $query->groupBy('visitors_city');
    $query->orderByHeader($header);
    $query->limit($items_per_page);
  
    $count_query = db_select('visitors', 'v');
    $count_query->addExpression('COUNT(DISTINCT visitors_city)');
    $count_query->condition('v.visitors_country_name', $original_country);
    visitors_date_filter_sql_condition($count_query);
    $query->setCountQuery($count_query);
    $results = $query->execute();
  
    $rows = array();
  
    $page = isset($_GET['page']) ? $_GET['page'] : '';
    $i = 0 + ($page  * $items_per_page);
  
    foreach ($results as $data) {
      if ($data->visitors_city == '') {
        $data->visitors_city = '(none)';
      }
      $rows[] = array(
        ++$i,
        l(
          $data->visitors_city,
          'visitors/countries/' . $country . '/' . $data->visitors_city
        ),
        $data->count
      );
    }
    return $rows;
  }
}

