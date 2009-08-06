<?php

// $Id$

/**
 * @file
 * Admin page callbacks for the visitors module.
 */

require_once 'pChart/pData.class';
require_once 'pChart/pChart.class';

/**
 * Draw graph.
 *
 * @values
 *   int array y-axis values
 * @x_array
 *   array x-axis params
 */
function visitors_graph($values, $x_array = NULL) {
  $width = (int) variable_get('visitors_graph_width', 700);
  $width = ($width <= 0) ? 700 : $width;

  $height = (int) variable_get('visitors_graph_height', 430);
  $height = ($height <=0) ? 430 : $height;

  // Dataset definition
  $data_set = new pData;
  $data_set->AddPoint($values, 'Serie1');

  if ($x_array !== NULL) {
    $data_set->AddPoint($x_array, 'Serie2');
    $data_set->SetAbsciseLabelSerie('Serie2');
  }

  $data_set->AddSerie('Serie1');
  $data_set->SetSerieName('', 'Serie1');

  // Initialise the graph
  $pchart = new pChart($width, $height);
  $pchart->setFontProperties(dirname(__FILE__) .'/Fonts/tahoma.ttf', 8);
  $pchart->setGraphArea(50, 30, $width - 20, $height - 30);
  $pchart->drawFilledRoundedRectangle(7, 7, $width - 7, $height - 7, 5, 240, 240, 240);
  $pchart->drawRoundedRectangle(5, 5, $width - 5, $height - 5, 5, 230, 230, 230);
  $pchart->drawGraphArea(255, 255, 255, TRUE);
  $pchart->setFixedScale(0, 0, 0, 0, 0, 0, 0);
  $pchart->drawScale($data_set->GetData(), $data_set->GetDataDescription(),
    SCALE_ADDALLSTART0, 150, 150, 150, TRUE, 0, 0, TRUE
                  );
  $pchart->drawGrid(4, TRUE, 230, 230, 230, 50);

  // Draw the 0 line
  $pchart->drawTreshold(0, 143, 55, 72, TRUE, TRUE);

  // Draw the bar graph
  $pchart->drawBarGraph($data_set->GetData(), $data_set->GetDataDescription(), TRUE);

  // Finish the graph
  $pchart->Stroke();
  exit();
}

/**
 * Menu callback; presents the "recent hits" page.
 */
function visitors_recent_hits() {
  $date_format    = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
  $items_per_page = variable_get('visitors_lines_per_page', 10);

  $header = array(
    array('data' => t('#')),
    array('data' => t('ID'), 'field' => 'visitors_id', 'sort' => 'desc'),
    array('data' => t('Date'), 'field' => 'visitors_date_time'),
    array('data' => t('URL'), 'field' => 'visitors_url'),
    array('data' => t('User'), 'field' => 'u.name'),
    array('data' => t('Operations'))
  );

  $sql = "SELECT v.*, u.name, u.uid
          FROM {visitors} v
          LEFT JOIN {users} u
          ON u.uid = v.visitors_uid". tablesort_sql($header);

  $sql_count = 'SELECT COUNT(*) FROM {visitors}';
  $results = pager_query($sql, $items_per_page, 0, $sql_count);

  $rows = array();

  $page = isset($_GET['page']) ? (int) $_GET['page'] : '';
  $i = 0 + ($page  * $items_per_page);

  while ($data = db_fetch_object($results)) {
    $user = user_load(array('uid' => $data->visitors_uid));
    $user_page = theme('username', $data);

    $rows[] = array(
      ++$i,
      $data->visitors_id,
      date($date_format, $data->visitors_date_time),
      check_plain($data->visitors_title) .'<br/>'. l($data->visitors_path,
      $data->visitors_url),
      $user_page,
      l(t('details'), 'admin/visitors/hits/'. $data->visitors_id)
    );
  }

  $output  = "";
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}

/**
 * Menu callback; presents the "hits from" page.
 */
function visitors_host_hits($ip) {
  if (!visitors_is_ip_valid($ip)) {
    return drupal_not_found();
  }

  $date_format    = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
  $items_per_page = variable_get('visitors_lines_per_page', 10);

  $header = array(
    array('data' => t('#')),
    array('data' => t('ID'), 'field' => 'visitors_id', 'sort' => 'desc'),
    array('data' => t('Date'), 'field' => 'visitors_date_time'),
    array('data' => t('URL'), 'field' => 'visitors_url'),
    array('data' => t('User'), 'field' => 'u.name'),
    array('data' => t('Operations'))
  );

  $sql = sprintf("SELECT v.*, u.name, u.uid
                  FROM {visitors} v
                  LEFT JOIN {users} u
                  ON u.uid = v.visitors_uid
                  WHERE v.visitors_ip='%u'" . tablesort_sql($header),
                  ip2long($ip));

  $count_sql = sprintf("SELECT COUNT(*) count
                        FROM {visitors}
                        WHERE visitors_ip='%u'",
                        ip2long($ip)
                      );

  $query = db_query($count_sql);
  $data = db_fetch_object($query);

  if ($data->count == 0) {
    return drupal_not_found();
  }

  $results = pager_query($sql, $items_per_page, 0, $count_sql);

  $rows = array();

  $page = isset($_GET['page']) ? (int) $_GET['page'] : '';
  $i = 0 + ($page  * $items_per_page);

  while ($data = db_fetch_object($results)) {
    $user = user_load(array('uid' => $data->visitors_uid));
    $user_page = theme('username', $data);
    $rows[] = array(
      ++$i,
      $data->visitors_id,
      date($date_format, $data->visitors_date_time),
      check_plain($data->visitors_title) .'<br/>'.
        l($data->visitors_path, $data->visitors_url),
      $user_page,
      l(t('details'), 'admin/visitors/hits/'. $data->visitors_id)
    );
  }

  $output  = "";
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  drupal_set_title(t('Hits from ') . check_plain($ip));

  return $output;
}

/**
 * Menu callback; presents the "hosts" page.
 */
function visitors_hosts() {
  $items_per_page = variable_get('visitors_lines_per_page', 10);

  $header = array(
    array('data' => t('#')),
    array('data' => t('Host'), 'field' => 'visitors_ip'),
    array('data' => t('Pages'), 'field' => 'count', 'sort' => 'desc'),
    array('data' => t('Operations'))
  );

  $sql = "SELECT count( * ) as count, visitors_ip
          FROM {visitors}
          GROUP BY visitors_ip" . tablesort_sql($header);

  $sql_count = "SELECT count(DISTINCT visitors_ip) FROM {visitors}";
  $results = pager_query($sql, $items_per_page, 0, $sql_count);

  $rows = array();

  $page = isset($_GET['page']) ? (int) $_GET['page'] : '';
  $i = 0 + ($page * $items_per_page);

  while ( $data = db_fetch_object($results) ) {
    $rows[] = array(
      ++$i,
      long2ip($data->visitors_ip),
      $data->count,
      l(t('hits'), 'admin/visitors/hosts/'. long2ip($data->visitors_ip))
    );
  }

  $output  = "";
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}

function visitors_hours() {
  $items_per_page = 24;

  $header = array(
    array('data' => t('#')),
    array('data' => t('Hour'), 'field' => 'hour', 'sort' => 'asc'),
    array('data' => t('Pages'), 'field' => 'count'),
  );

  $sql = sprintf('select count(*) count,
                  %s as  hour from {visitors} group by hour %s',
                  visitors_date_format_sql('visitors_date_time', '%H'),
                  tablesort_sql($header)
                );

  $sql_count = sprintf('SELECT count(distinct(%s)) FROM {visitors}',
                        visitors_date_format_sql('visitors_date_time', '%H'));

  $results = pager_query($sql, $items_per_page, 0, $sql_count);

  $rows = array();

  $page = isset($_GET['page']) ? (int) $_GET['page'] : '';
  $i = 0 + $page * $items_per_page;

  while ($data = db_fetch_object($results)) {
    $rows[] = array(
      ++$i,
      $data->hour,
      $data->count
    );
  }

  $output  = '<img src="'. url() .'admin/visitors/hours/graph" alt="Hours">';
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}

function graph_visitors_hours() {
  $sql = sprintf('SELECT count(*) count,
                  %s as hour
                  FROM {visitors}
                  group by hour',
                  visitors_date_format_sql('visitors_date_time', '%H')
                );
  $result = db_query($sql);
  $tmp_rows = array();
  $rows = array();
  for ($i = 0; $i < 24; $i++) {
    $rows[$i] = 0;
  }

  while ( $data = db_fetch_object($result) ) {
    $rows[(int)$data->hour] = $data->count;
  }

  $hours = range(0, 23);

  visitors_graph($rows, $hours);
}

function visitors_days_of_week() {
  $header = array(t('#'), t('Day'), t('Pages'));

  $sql = sprintf('select count(*) as count,
                  %s as d,
                  %s as n
                  from {visitors}
                  group by d
                  order by %s',
                  visitors_date_format_sql('visitors_date_time', '%a'),
                  visitors_date_format_sql('min(visitors_date_time)', '%w'),
                  visitors_date_format_sql('min(visitors_date_time)', '%w')
                );

  $result = db_query($sql);
  $tmp_rows = array();

  while ($data = db_fetch_object($result)) {
    $tmp_rows[$data->n] = array(
      $data->d,
      $data->count,
      $data->n
    );
  }

  $rows = array(
    array(1, t('Sun'), 0),
    array(2, t('Mon'), 0),
    array(3, t('Tue'), 0),
    array(4, t('Wed'), 0),
    array(5, t('Thu'), 0),
    array(6, t('Fri'), 0),
    array(7, t('Sat'), 0),
  );

  foreach ($tmp_rows as $key => $tmp_item) {
    $rows[$key][2] = $tmp_item[1];
  }

  $output  = '<img src="'. url() .'admin/visitors/days_of_week/graph" alt="Days of week">';
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}

function graph_visitors_days_of_week() {
  $sql = sprintf('select count(*) as count,
                  %s as d,
                  %s as n
                  from {visitors}
                  group by d
                  order by %s',
                  visitors_date_format_sql('visitors_date_time', '%a'),
                  visitors_date_format_sql('min(visitors_date_time)', '%w'),
                  visitors_date_format_sql('min(visitors_date_time)', '%w')
                );

  $result = db_query($sql);
  $tmp_rows = array();

  while ($data = db_fetch_object($result)) {
    $tmp_rows[$data->n] = array(
      $data->d,
      $data->count,
      $data->n
    );
  }

  $rows = array();
  for ($i = 0; $i < 7; $i++) {
    $rows[$i] = 0;
  }

  foreach ($tmp_rows as $key => $tmp_item) {
    $rows[$key] = (int)$tmp_item[1];
  }

 // build dates series
  $dates = array(
    t('Sun'), t('Mon'), t('Tue'), t('Wed'),
    t('Thu'), t('Fri'), t('Sat')
  );

  visitors_graph($rows, $dates);
}

function visitors_days_of_month() {
  $items_per_page = 31;

  $header = array(
    array('data' => t('#')),
    array('data' => t('Day'), 'field' => 'day', 'sort' => 'asc'),
    array('data' => t('Pages'), 'field' => 'count'),
  );

  global $db_type;
  if ($db_type == 'pgsql') {
    $sql = sprintf('select count(*) count,
                    %s as day
                    from {visitors}
                    group by day %s',
                    visitors_date_format_sql('visitors_date_time', '%e'),
                    tablesort_sql($header)
                  );
  }
  else {
    $sql = sprintf('select count(*) count,
                    %s+0 as day
                    from {visitors}
                    group by day %s',
                    visitors_date_format_sql('visitors_date_time', '%e'),
                    tablesort_sql($header)
                  );
  }
  $result = db_query($sql);

  $rows = array();
  $i = 0;

  while ($data = db_fetch_object($result)) {
    $rows[] = array(
      ++$i,
      (int) $data->day,
      $data->count
    );
  }

  $output  = '<img src="'. url() .'admin/visitors/days_of_month/graph" alt="Days of month">';
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}

function graph_visitors_days_of_month() {
  $sql = sprintf('select count(*) count,
                  %s as day
                  from {visitors}
                  group by day',
                  visitors_date_format_sql('visitors_date_time', '%e')
                );

  $results = pager_query($sql, 31, 0, NULL);
  $rows = array();

  for ($i = 1; $i <= 31; $i++) {
    $rows[$i] = 0;
  }

  while ($data = db_fetch_object($results)) {
    $rows[(int)$data->day] = (int)$data->count;
  }

  // build dates series
  $dates = range(1, 31);

  visitors_graph($rows, $dates);
}

function visitors_monthly_history() {
  $items_per_page = variable_get('visitors_lines_per_page', 10);

  $header = array(
    array('data' => t('#')),
    array('data' => t('Month'), 'field' => 'm', 'sort' => 'asc'),
    array('data' => t('Pages'), 'field' => 'count'),
  );

  $sql = sprintf('select count(*) count,
                  %s as m,
                  %s as s
                  from {visitors}
                  group by m %s',
                  visitors_date_format_sql('visitors_date_time', '%Y%m'),
                  visitors_date_format_sql('MIN(visitors_date_time)', '%Y %M'),
                  tablesort_sql($header)
                );

  $sql_count = sprintf('SELECT count(DISTINCT %s)
                        FROM {visitors}',
                        visitors_date_format_sql('visitors_date_time', '%Y %M')
                      );

  $results = pager_query($sql, $items_per_page, 0, $sql_count);

  $rows = array();

  $page = isset($_GET['page']) ? (int) $_GET['page'] : '';
  $i = 0 + ($page  * $items_per_page);


  while ($data = db_fetch_object($results)) {
    $rows[] = array(
      ++$i,
      $data->s,
      $data->count
    );
  }

  if (count($rows) > 1) {
    $output  = '<img src="'. url() .'admin/visitors/monthly_history/graph" alt="Monthly history">';
  }

  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}

function graph_visitors_monthly_history() {
  $sql = sprintf('select count(*) count,
                  %s as m,
                  %s as s
                  from {visitors}
                  group by m
                  order by m ASC',
                  visitors_date_format_sql('visitors_date_time', '%Y%m'),
                  visitors_date_format_sql('MIN(visitors_date_time)', '%Y %M')
                );

  $rows = array();
  $dates = array();

  $result = db_query($sql);

  while ($data = db_fetch_object($result)) {
    $rows[$data->s] = (int) $data->count;
    $dates[] = $data->s;
  }

  if (count($rows) > 1) {
    visitors_graph($rows, $dates);
  }
}

/**
 * Menu callback; Displays recent page accesses.
 */
function visitors_hit_details($aid) {
  $result = db_query('SELECT a.*, u.name, u.uid
                      FROM {visitors} a
                      LEFT JOIN {users} u
                      ON a.visitors_uid = u.uid
                      WHERE visitors_id = %d',
                      $aid
                    );

  if ($access = db_fetch_object($result)) {

    $rows[] = array(
      array('data' => t('URL'), 'header' => TRUE),
      l(urldecode(url($access->visitors_path, array('absolute' => TRUE))), $access->visitors_path)
    );

    $rows[] = array(
      array('data' => t('Title'), 'header' => TRUE),
      check_plain($access->visitors_title)
    );

    $rows[] = array(
      array('data' => t('Referrer'), 'header' => TRUE),
      ($access->visitors_referer ? l($access->visitors_referer, $access->visitors_referer) : '')
    );

    $rows[] = array(
      array('data' => t('Date'), 'header' => TRUE),
      format_date($access->visitors_date_time, 'large')
    );

    $rows[] = array(
      array('data' => t('User'), 'header' => TRUE),
      theme('username', $access)
    );

    $rows[] = array(
      array('data' => t('Hostname'), 'header' => TRUE),
      check_plain(long2ip($access->visitors_ip))
    );

    $rows[] = array(
      array('data' => t('User Agent'), 'header' => TRUE),
      check_plain($access->visitors_user_agent)
    );

    return theme('table', array(), $rows);
  }
  else {
    drupal_not_found();
  }
}

function visitors_top_pages() {
  $date_format = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
  $items_per_page = variable_get('visitors_lines_per_page', 10);

  $header = array(
    array('data' => t('#')),
    array('data' => t('URL'), 'field' => 'visitors_url'),
    array('data' => t('count'), 'field' => 'count', 'sort' => 'desc'),
  );

  $sql = "SELECT COUNT(visitors_id) AS count,
                 visitors_path,
                 MIN(visitors_title) AS visitors_title,
                 MIN(visitors_url) AS visitors_url
          FROM {visitors} GROUP BY visitors_path" . tablesort_sql($header);

  $sql_count = 'SELECT COUNT(DISTINCT visitors_path) FROM {visitors}';

  $results = pager_query($sql, $items_per_page, 0, $sql_count);

  $rows = array();

  $page = isset($_GET['page']) ? $_GET['page'] : '';
  $i = 0 + ($page  * $items_per_page);

  while ($data = db_fetch_object($results)) {
    $rows[] = array(
      ++$i,
      check_plain($data->visitors_title) .'<br/>'.
      l($data->visitors_path, $data->visitors_url),
      $data->count,
    );
  }

  $output  = "";
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}

function visitors_user_activity() {
  $date_format    = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
  $items_per_page = variable_get('visitors_lines_per_page', 10);

  $header = array(
    array('data' => t('#')),
    array('data' => t('User'), 'field' => 'name'),
    array('data' => t('Hits'), 'field' => 'hits', 'sort' => 'desc'),
    array('data' => t('Nodes'), 'field' => 'nodes'),
    array('data' => t('Comments'), 'field' => 'comments')
  );

  $sql = "SELECT
            i.name,
            i.uid,
            i.hits,
            i.nodes,
            count(c.cid) as comments
          FROM (
            SELECT
              h.name,
              h.uid,
              h.hits,
              count(n.nid) as nodes
            FROM (
              SELECT
                u.name,
                MIN(u.uid) as uid,
                count(v.visitors_id) as hits
              FROM {users} u
              LEFT JOIN {visitors} v
              ON v.visitors_uid= u.uid
              GROUP BY u.name
            ) AS h
            LEFT JOIN {node} n
            ON h.uid=n.uid
            GROUP BY h.name, h.uid, h.hits
          ) AS i
          LEFT JOIN {comments} c
          ON c.uid=i.uid
          GROUP BY i.name, i.uid, i.hits, i.nodes
          " . tablesort_sql($header);

  $sql_count = 'SELECT COUNT(*) FROM {users}';
  $results = pager_query($sql, $items_per_page, 0, $sql_count);

  $rows = array();

  $page = isset($_GET['page']) ? (int) $_GET['page'] : '';
  $i = 0 + ($page  * $items_per_page);

  while ($data = db_fetch_object($results)) {
    $user = user_load(array('uid' => $data->uid));
    $user_page = theme('username', $data);

    $rows[] = array(
      ++$i,
      $user_page,
      $data->hits,
      $data->nodes,
      $data->comments
    );
  }

  $output  = "";
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}


