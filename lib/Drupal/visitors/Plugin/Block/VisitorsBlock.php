<?php

/**
 * @file
 * Contains \Drupal\visitors\Plugin\Block\VisitorsBlock.
 */

namespace Drupal\visitors\Plugin\Block;

use Drupal\block\BlockBase;

/**
 * Provides a 'Visitors' block.
 *
 * @Block(
 *   id = "visitors_block",
 *   admin_label = @Translation("Visitors"),
 *   category = @Translation("Visitors")
 * )
 */
class VisitorsBlock extends BlockBase {
  protected $config;
  protected $items;

  /**
   * {@inheritdoc}
   */
  public function build() {
    $this->config = \Drupal::config('visitors.config');
    $this->items = array();

    $this->_showTotalVisitors();
    $this->_showUniqueVisitors();
    $this->_showRegisteredUsersCount();
    $this->_showLastRegisteredUser();
    $this->_showPublishedNodes();
    $this->_showUserIp();
    $this->_showSinceDate();

    return array(
      'visitors_info' => array(
        '#theme' => 'item_list',
        '#items' => $this->items,
      ),
    );
  }

  /**
   * @TODO: rewrite sql.
   * Display total visitors count to visitors block.
   */
  protected function _showTotalVisitors() {
    if ($this->config->get('show_total_visitors')) {
      $query = 'SELECT COUNT(*) FROM {visitors}';
      $count = db_query($query)->fetchField() + $this->config->get('start_count_total_visitors');
      $this->items[] = t('Total Visitors: %visitors',
        array('%visitors' => $count)
      );
    }
  }

  /**
   * @TODO: rewrite sql.
   * Display unique visitors count to visitors block.
   */
  protected function _showUniqueVisitors() {
    if ($this->config->get('show_unique_visitor')) {
      $query = 'SELECT COUNT(DISTINCT visitors_ip) FROM {visitors}';
      $unique_visitors = db_query($query)->fetchField();
      $this->items[] = t('Unique Visitors: %unique_visitors',
        array('%unique_visitors' => $unique_visitors)
      );
    }
  }

  /**
   * Display registered users count to visitors block.
   */
  protected function _showRegisteredUsersCount() {
    if ($this->config->get('show_registered_users_count')) {
      $query = db_select('users');
      $query->addExpression('COUNT(*)', 'count');
      $query->condition('uid', '0', '>');

      $registered_users_count = $query->execute()->fetchField();

      $this->items[] = t('Registered Users: %registered_users_count',
        array('%registered_users_count' => $registered_users_count)
      );
    }
  }

  /**
   * Display last registered user to visitors block.
   */
  protected function _showLastRegisteredUser() {
    if ($this->config->get('show_last_registered_user')) {
      $last_user_uid = db_select('users', 'u')
        ->fields('u', array('uid'))
        ->orderBy('uid', 'DESC')
        ->range(0, 1)
        ->execute()
        ->fetchField();
      $user = user_load($last_user_uid);
      $username = array(
        '#theme' => 'username',
        '#account' => $user
      );

      $this->items[] = t('Last Registered User: !last_user',
        array('!last_user' => drupal_render($username))
      );
    }
  }

  /**
   * Display published nodes count to visitors block.
   */
  protected function _showPublishedNodes() {
    if ($this->config->get('show_published_nodes')) {
      $query = db_select('node', 'n');
      $query->innerJoin('node_field_data', 'nfd', 'n.nid = nfd.nid');
      $query->addExpression('COUNT(*)', 'count');
      $query->condition('nfd.status', '1', '=');

      $nodes = $query->execute()->fetchField();
      $this->items[] = t('Published Nodes: %nodes',
        array('%nodes' => $nodes)
      );
    }
  }

  /**
   * Display user ip to visitors block.
   */
  protected function _showUserIp() {
    if ($this->config->get('show_user_ip')) {
      $this->items[] = t('Your IP: %user_ip',
        array('%user_ip' => \Drupal::request()->getClientIp())
      );
    }
  }

  /**
   * @todo: rewrite sql.
   * Display the start date statistics to visitors block.
   */
  protected function _showSinceDate() {
    if ($this->config->get('show_since_date')) {
      $query = 'SELECT MIN(visitors_date_time)
        FROM {visitors}
        ORDER BY visitors_date_time ASC LIMIT 1';
      $since_date = db_query($query)->fetchField();
      $this->items[] = t('Since: %since_date',
        array('%since_date' => format_date($since_date, 'short'))
      );
    }
  }
}

