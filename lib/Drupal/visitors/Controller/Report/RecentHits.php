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

     $list['visitors_table'] = array(
      '#theme' => 'table',
      '#header' => array(
        '#'                  => array('data' => t('#')),
        'visitors_id'        => array('data' => t('ID')),
        'visitors_date_time' => array('data' => t('Date')),
        'visitors_url'       => array('data' => t('URL')),
        'u.name'             => array('data' => t('User'))
      ),
      '#rows' => array(),
    );

    $list['visitors']['#empty'] = '';

    return $list;
  }
}

