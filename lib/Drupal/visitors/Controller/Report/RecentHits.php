<?php

namespace Drupal\visitors\Controller\Report;

use Drupal\Component\Utility\String;
use Drupal\Component\Utility\Unicode;
use Drupal\Component\Utility\Xss;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Database\Connection;
use Drupal\Core\Datetime\Date;
use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\visitors\Form;
use Drupal\visitors\Form\DateFilter;
use Symfony\Component\DependencyInjection\ContainerInterface;

class RecentHits extends ControllerBase  {
  /**
   * The database service.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $database;

  /**
   * The module handler service.
   *
   * @var \Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $moduleHandler;

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
      $container->get('database'),
      $container->get('module_handler'),
      $container->get('date'),
      $container->get('form_builder')
    );
  }

  /**
   * Constructs a RecentHits object.
   *
   * @param \Drupal\Core\Database\Connection $database
   *   A database connection.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   A module handler.
   * @param \Drupal\Core\Datetime\Date $date
   *   The date service.
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder service.
   */
  public function __construct(
    Connection $database,
    ModuleHandlerInterface $module_handler,
    Date $date,
    FormBuilderInterface $form_builder
  ) {
    $this->database      = $database;
    $this->moduleHandler = $module_handler;
    $this->date          = $date;
    $this->formBuilder   = $form_builder;
  }

  /**
   */
  public function display() {
    $list = array();

    $list['visitors'] = array(
      '#theme' => 'report',
    );
    $list['visitors_date_filter_form'] = $this->formBuilder->getForm('Drupal\visitors\Form\DateFilter');

    $header = $this->_getHeader();
     $list['visitors_table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $this->_getData($header),
    );

    $list['visitors']['#empty'] = '';

    return $list;
  }

  protected function _getHeader() {
    return array(
        '#'                  => array('data' => t('#')),
        'visitors_id'        => array('data' => t('ID')),
        'visitors_date_time' => array('data' => t('Date')),
        'visitors_url'       => array('data' => t('URL')),
        'u.name'             => array('data' => t('User')),
        ''                   => array('data' => t('Details')),
      );
  }

  protected function _getData($header) {
    $items_per_page = 10;
    $query = db_select('visitors', 'v')
    ->extend('Drupal\Core\Database\Query\PagerSelectExtender')
    ->extend('Drupal\Core\Database\Query\TableSortExtender');
  $query->leftJoin('users', 'u', 'u.uid=v.visitors_id');
  $query->fields(
    'v',
    array(
      'visitors_id',
      'visitors_uid',
      'visitors_date_time',
      'visitors_title',
      'visitors_path',
      'visitors_url'
    )
  );
  $query->fields('u', array('name', 'uid'));
  visitors_date_filter_sql_condition($query);
  $query->orderByHeader($header);
  $query->limit($items_per_page);

  $count_query = db_select('visitors', 'v');
  $count_query->addExpression('COUNT(*)');
  visitors_date_filter_sql_condition($count_query);
  $query->setCountQuery($count_query);
  $results = $query->execute();

  $rows = array();

  $page = isset($_GET['page']) ? (int) $_GET['page'] : '';
  $i = 0 + ($page  * $items_per_page);
  $timezone =  drupal_get_user_timezone();

  foreach ($results as $data) {
    $user = user_load($data->visitors_uid);
    $user_page = '';//theme('username', array('account' => $user));

    $rows[] = array(
      ++$i,
      $data->visitors_id,
      format_date(
        $data->visitors_date_time,
        'custom',
        $date_format,
        $timezone
      ),
      check_plain(
        $data->visitors_title) . '<br/>' . l($data->visitors_path,
        $data->visitors_url
      ),
      $user_page,
      l(t('details'), 'visitors/hits/' . $data->visitors_id)
    );
  }
   return $rows;
  }
}
