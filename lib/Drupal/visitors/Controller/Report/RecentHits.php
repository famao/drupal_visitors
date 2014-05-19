<?php

namespace Drupal\visitors\Controller\Report;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Database\Connection;
use Drupal\Core\Datetime\Date;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Form\FormBuilderInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class RecentHits extends ControllerBase  {
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
    return new static($container->get('form_builder'));
  }

  /**
   * Constructs a RecentHits object.
   *
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder service.
   */
  public function __construct(FormBuilderInterface $form_builder) {
    $this->formBuilder = $form_builder;
  }

  /**
   */
  public function display() {
    $form = $this->formBuilder->getForm('Drupal\visitors\Form\DateFilter');
    $header = $this->_getHeader();

    return array(
      'visitors' => array(
        '#theme' => 'report',
        '#empty' => '',
      ),
      'visitors_date_filter_form' => $form,
      'visitors_table' => array(
        '#theme'  => 'table',
        '#header' => $header,
        '#rows'   => $this->_getData($header),
      ),
      'visitors_pager' => array('#theme' => 'pager')
    );
  }

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
        'data'      => t('Details'),
      ),
    );
  }

  protected function _getData($header) {
    $items_per_page = \Drupal::config('visitors.config')->get('items_per_page');
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
      $username = array(
        '#theme' => 'username',
        '#account' => $user
      );

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
        drupal_render($username),
        l(t('details'), 'visitors/hits/' . $data->visitors_id)
      );
    }

    return $rows;
  }
}

