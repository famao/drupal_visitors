<?php

/**
 * @file
 * Contains Drupal\visitors\Controller\Report\HitDetails.
 */

namespace Drupal\visitors\Controller\Report;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Datetime\DateFormatter;
use Drupal\Component\Utility\Html;
use Drupal\Core\Url;
use Symfony\Component\DependencyInjection\ContainerInterface;

class HitDetails extends ControllerBase {
  /**
   * The date service.
   *
   * @var \Drupal\Core\Datetime\DateFormatter
   */
  protected $date;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('date.formatter'));
  }

  /**
   * Constructs a HitDetails object.
   *
   * @param \Drupal\Core\Datetime\DateFormatter $date
   *   The date service.
   */
  public function __construct(DateFormatter $date) {
    $this->date = $date;
  }

  /**
   * Returns a hit details page.
   *
   * @return array
   *   A render array representing the hit details page content.
   */
  public function display($hit_id) {
    return array(
      'visitors_table' => array(
        '#theme' => 'table',
        '#rows'  => $this->_getData($hit_id),
      ),
    );
  }

  /**
   * Returns a table content.
   *
   * @param int $hit_id
   *   Unique id of the visitors log.
   *
   * @return array
   *   Array representing the table content.
   */
  protected function _getData($hit_id) {
    $query = db_select('visitors', 'v');
    $query->leftJoin('users_field_data', 'u', 'u.uid=v.visitors_uid');
    $query->fields('v');
    $query->fields('u', array('name', 'uid'));
    $query->condition('v.visitors_id', (int) $hit_id);
    $hit_details = $query->execute()->fetch();

    $rows = array();

    if ($hit_details) {
      $url          = urldecode($hit_details->visitors_url);
      $referer      = $hit_details->visitors_referer;
      $date         = $this->date->format($hit_details->visitors_date_time, 'large');
      $whois_enable = \Drupal::moduleHandler()->moduleExists('whois');

      $attr         = array(
        'attributes' => array(
          'target' => '_blank',
          'title'  => t('Whois lookup')
        )
      );
      $ip = long2ip($hit_details->visitors_ip);
      $user = user_load($hit_details->visitors_uid);
      $username = array(
        '#theme'   => 'username',
        '#account' => $user
      );

      $array = array(
        'URL'        => \Drupal::l($url, Url::fromUri($url)),
        'Title'      => Html::escape($hit_details->visitors_title),
        'Referer'    => $referer ? \Drupal::l($referer, Url::fromUri($referer)) : '',
        'DateFormatter'       => $date,
        'User'       => drupal_render($username),
        'IP'         => $whois_enable ? \Drupal::l($ip, Url::fromUri('whois/' . $ip), $attr) : $ip,
        'User Agent' => Html::escape($hit_details->visitors_user_agent)
      );

      if (\Drupal::moduleHandler()->moduleExists('visitors_geoip')) {
        $geoip_data_array = array(
          'Country'        => Html::escape($hit_details->visitors_country_name),
          'Region'         => Html::escape($hit_details->visitors_region),
          'City'           => Html::escape($hit_details->visitors_city),
          'Postal Code'    => Html::escape($hit_details->visitors_postal_code),
          'Latitude'       => Html::escape($hit_details->visitors_latitude),
          'Longitude'      => Html::escape($hit_details->visitors_longitude),
          'DMA Code'       => Html::escape($hit_details->visitors_dma_code),
          'PSTN Area Code' => Html::escape($hit_details->visitors_area_code),
        );
        $array = array_merge($array, $geoip_data_array);
      }

      foreach ($array as $key => $value) {
        $rows[] = array(array('data' => t($key), 'header' => TRUE), $value);
      }
    }

    return $rows;
  }
}

