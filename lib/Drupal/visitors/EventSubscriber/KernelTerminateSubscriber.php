<?

/**
 * @file
 * Contains Drupal\visitors\EventSubscriber\KernelTerminateSubscriber.
 */

namespace Drupal\visitors\EventSubscriber;

use Drupal;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\CronInterface;
use Drupal\Core\State\StateInterface;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Symfony\Component\HttpKernel\Event\PostResponseEvent;
use Symfony\Component\HttpKernel\KernelEvents;
use Symfony\Cmf\Component\Routing\RouteObjectInterface;
use Symfony\Component\HttpFoundation\Request;
use Drupal\Core\Site\Settings;
use Drupal\Core\Page\DefaultHtmlPageRenderer;

/**
 * A subscriber running cron when a request terminates.
 */
class KernelTerminateSubscriber implements EventSubscriberInterface {
  /**
   * The cron configuration.
   *
   * @var \Drupal\Core\Config\Config
   */
  protected $config;

  /**
   * The state key value store.
   *
   * Drupal\Core\State\StateInterface;
   */
  protected $state;

  protected $request;
  /**
   * Run the automated cron if enabled.
   *
   * @param Symfony\Component\HttpKernel\Event\PostResponseEvent $event
   *   The Event to process.
   */
  public function onTerminate(PostResponseEvent $event) {
    $this->request = $event->getRequest();
   drupal_bootstrap(DRUPAL_BOOTSTRAP_SESSION);

  global $user;
  $not_admin = !in_array('administrator', $user->getRoles());
  $log_admin = !\Drupal::config('visitors.config')->get('exclude_administer_users');

  if ($log_admin || $not_admin) {
    $ip_str = visitors_get_ip_str();

    $fields = array(
      'visitors_uid'            => $user->id(),
      'visitors_ip'             => $ip_str,
      'visitors_date_time'      => time(),
      'visitors_url'            => visitors_get_url(),
      'visitors_referer'        => visitors_get_referer(),
      'visitors_path'           => visitors_get_path(),
      'visitors_title'          => $this->_getTitle(),
      'visitors_user_agent'     => visitors_get_user_agent()
    );

    if (module_exists('visitors_geoip')) {
      $geoip_data = visitors_get_geoip_data($ip_str);

      $fields['visitors_continent_code'] = $geoip_data['continent_code'];
      $fields['visitors_country_code']   = $geoip_data['country_code'];
      $fields['visitors_country_code3']  = $geoip_data['country_code3'];
      $fields['visitors_country_name']   = $geoip_data['country_name'];
      $fields['visitors_region']         = $geoip_data['region'];
      $fields['visitors_city']           = $geoip_data['city'];
      $fields['visitors_postal_code']    = $geoip_data['postal_code'];
      $fields['visitors_latitude']       = $geoip_data['latitude'];
      $fields['visitors_longitude']      = $geoip_data['longitude'];
      $fields['visitors_dma_code']       = $geoip_data['dma_code'];
      $fields['visitors_area_code']      = $geoip_data['area_code'];
    }

    db_insert('visitors')
      ->fields($fields)
      ->execute();
    }
  }

  /**
   * Registers the methods in this class that should be listeners.
   *
   * @return array
   *   An array of event listener definitions.
   */
  public static function getSubscribedEvents() {
    $events[KernelEvents::TERMINATE][] = array('onTerminate', 100);

    return $events;
  }

  /**
   * Get the title of the current page.
   *
   * @return string
   *   title of the current page
   */
  protected function _getTitle() {
    if ($route = $this->request->attributes->get(RouteObjectInterface::ROUTE_OBJECT)) {
      $title = \Drupal::service('title_resolver')->getTitle($this->request, $route);
      return htmlspecialchars_decode($title, ENT_QUOTES);
    }

    return '';
  }
}

