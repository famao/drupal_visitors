<?php

/**
 * @file
 * Contains Drupal\visitors\Controller\Report\Hosts.
 */

namespace Drupal\visitors\Controller\Report;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Datetime\Date;
use Drupal\Core\Form\FormBuilderInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class Hosts extends ControllerBase {
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
   * Constructs a Hosts object.
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
   * Returns a hosts page.
   *
   * @return array
   *   A render array representing the hosts page content.
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
      'visitors_ip' => array(
        'data'      => t('Host'),
        'field'     => 'visitors_ip',
        'specifier' => 'visitors_ip',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
      ),
      'count' => array(
        'data'      => t('Count'),
        'field'     => 'count',
        'specifier' => 'count',
        'class'     => array(RESPONSIVE_PRIORITY_LOW),
        'sort'      => 'desc',
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
  protected function _getData($header) {
    $items_per_page = \Drupal::config('visitors.config')->get('items_per_page');

    $query = db_select('visitors', 'v')
      ->extend('Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('Drupal\Core\Database\Query\TableSortExtender');

    $query->addExpression('COUNT(*)', 'count');
    $query->fields('v', array('visitors_ip'));
    visitors_date_filter_sql_condition($query);
    $query->groupBy('visitors_ip');
    $query->orderByHeader($header);
    $query->limit($items_per_page);

    $count_query = db_select('visitors', 'v');
    $count_query->addExpression('COUNT(DISTINCT visitors_ip)');
    visitors_date_filter_sql_condition($count_query);
    $query->setCountQuery($count_query);
    $results = $query->execute();

    $whois_enable = module_exists('whois');
    $attr = array('attributes' =>
      array('target' => '_blank', 'title' => t('Whois lookup'))
    );

    $rows = array();

    $page = isset($_GET['page']) ? $_GET['page'] : '';
    $i = 0 + $page * $items_per_page;

    foreach ($results as $data) {
      $ip = long2ip($data->visitors_ip);
      $rows[] = array(
        ++$i,
        $whois_enable ? l($ip, 'whois/' . $ip, $attr) : check_plain($ip),
        $data->count,
        l(t('hits'), 'visitors/hosts/' . $ip)
      );
    }

    return $rows;
  }
}

