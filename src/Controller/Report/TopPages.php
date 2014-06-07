<?php

/**
 * @file
 * Contains Drupal\visitors\Controller\Report\TopPages.
 */

namespace Drupal\visitors\Controller\Report;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Datetime\Date;
use Drupal\Core\Form\FormBuilderInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class TopPages extends ControllerBase {
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
   * Constructs a TopPages object.
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
   * Returns a top pages page.
   *
   * @return array
   *   A render array representing the top pages page content.
   */
  public function display() {
    $form = $this->formBuilder->getForm('Drupal\visitors\Form\DateFilter');
    $header = $this->_getHeader();

    return array(
      'visitors_date_filter_form' => $form,
      'visitors_table' => array(
        '#theme'  => 'table',
        '#header' => $header,
        '#rows'   => $this->_getData($header),
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
      'visitors_url' => array(
        'data'      => t('URL'),
        'field'     => 'visitors_url',
        'specifier' => 'visitors_url',
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
  protected function _getData($header) {
    $items_per_page = \Drupal::config('visitors.config')->get('items_per_page');

    $query = db_select('visitors', 'v')
      ->extend('Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('Drupal\Core\Database\Query\TableSortExtender');

    $query->addExpression('COUNT(visitors_id)', 'count');
    $query->addExpression('MIN(visitors_title)', 'visitors_title');
    $query->addExpression('MIN(visitors_url)', 'visitors_url');
    $query->fields('v', array('visitors_path'));
    visitors_date_filter_sql_condition($query);
    $query->groupBy('visitors_path');
    $query->orderByHeader($header);
    $query->limit($items_per_page);

    $count_query = db_select('visitors', 'v');
    $count_query->addExpression('COUNT(DISTINCT visitors_path)');
    visitors_date_filter_sql_condition($count_query);
    $query->setCountQuery($count_query);
    $results = $query->execute();

    $rows = array();

    $page = isset($_GET['page']) ? $_GET['page'] : '';
    $i = 0 + $page * $items_per_page;

    foreach ($results as $data) {
      $rows[] = array(
        ++$i,
        check_plain($data->visitors_title) . '<br/>' .
        l($data->visitors_path, $data->visitors_url),
        $data->count,
      );
    }

    return $rows;
  }
}

