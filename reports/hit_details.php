<?php

/**
 * @file
 * Hit details report for the visitors module.
 */

/**
 * Menu callback; Displays recent page accesses.
 *
 * @param visitors_id
 *   int visitors id from visitors table
 *
 * @return
 *   string hit details report or 404 error if visitors_id not found
 */
function visitors_hit_details($visitors_id) {
  $query = db_select('visitors', 'v');
  $query->leftJoin('users', 'u', 'u.uid=v.visitors_uid');
  $query->fields(
    'v',
    array(
      'visitors_url',
      'visitors_title',
      'visitors_referer',
      'visitors_date_time',
      'visitors_ip',
      'visitors_user_agent'
    )
  );
  $query->fields('u', array('name', 'uid'));
  $query->condition('v.visitors_id', (int)$visitors_id);
  $hit_details = $query->execute()->fetch();

  if ($hit_details) {
    $rows[] = array(
      array('data' => t('URL'), 'header' => TRUE),
      l(
        urldecode($hit_details->visitors_url),
        urldecode($hit_details->visitors_url)
      )
    );

    $rows[] = array(
      array('data' => t('Title'), 'header' => TRUE),
      check_plain($hit_details->visitors_title)
    );

    $rows[] = array(
      array('data' => t('Referer'), 'header' => TRUE),
      ($hit_details->visitors_referer ?
        l($hit_details->visitors_referer, $hit_details->visitors_referer) : ''
      )
    );

    $rows[] = array(
      array('data' => t('Date'), 'header' => TRUE),
      format_date($hit_details->visitors_date_time, 'large', visitors_get_timezone())
    );

    $rows[] = array(
      array('data' => t('User'), 'header' => TRUE),
      theme('username', array('account' => $hit_details))
    );

    $whois_enable = module_exists('whois');
    $attr = array(
      'attributes' => array('target' => '_blank', 'title' => t('Whois lookup'))
    );
    $ip = long2ip($hit_details->visitors_ip);

    $rows[] = array(
      array('data' => t('Hostname'), 'header' => TRUE),
      $whois_enable ? l($ip, 'whois/' . $ip, $attr) : check_plain($ip)
    );

    $rows[] = array(
      array('data' => t('User Agent'), 'header' => TRUE),
      check_plain($hit_details->visitors_user_agent)
    );

    return theme('table', array('rows' => $rows));
  }

  drupal_not_found();
}

