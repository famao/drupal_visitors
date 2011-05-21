<?php

/**
 * @file
 * Admin page callbacks for the visitors module.
 */

require_once dirname(__FILE__) .'/pchart/pData.class';
require_once dirname(__FILE__) .'/pchart/pChart.class';

require_once dirname(__FILE__) .'/forms/date_filter.php';
require_once dirname(__FILE__) .'/forms/referers.php';

require_once dirname(__FILE__) .'/reports/days_of_month.php';
require_once dirname(__FILE__) .'/reports/days_of_week.php';
require_once dirname(__FILE__) .'/reports/hit_details.php';
require_once dirname(__FILE__) .'/reports/hosts.php';
require_once dirname(__FILE__) .'/reports/hours.php';
require_once dirname(__FILE__) .'/reports/monthly_history.php';
require_once dirname(__FILE__) .'/reports/recent_hits.php';
require_once dirname(__FILE__) .'/reports/referers.php';

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

  $data = $data_set->GetData();
  $data_description = $data_set->GetDataDescription();

  // Initialise the graph
  $pchart = new pChart($width, $height);
  $pchart->setFontProperties(dirname(__FILE__) .'/fonts/tahoma.ttf', 8);
  $pchart->setGraphArea(50, 30, $width - 20, $height - 30);
  $pchart->drawFilledRoundedRectangle(7, 7, $width - 7, $height - 7, 5, 240, 240, 240);
  $pchart->drawRoundedRectangle(5, 5, $width - 5, $height - 5, 5, 230, 230, 230);
  $pchart->drawGraphArea(255, 255, 255, TRUE);
  $pchart->setFixedScale(0, 0, 0, 0, 0, 0, 0);
  $pchart->drawScale($data, $data_description,
    SCALE_ADDALLSTART0, 150, 150, 150, TRUE, 0, 0, TRUE
  );
  $pchart->drawGrid(4, TRUE, 230, 230, 230, 50);

  // Draw the 0 line
  $pchart->drawTreshold(0, 143, 55, 72, TRUE, TRUE);

  // Draw the bar graph
  $pchart->drawBarGraph($data, $data_description, TRUE);

  // Finish the graph
  $pchart->Stroke();
  exit();
}

function visitors_top_pages() {
  $date_format = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
  $items_per_page = variable_get('visitors_lines_per_page', 10);

  $header = array(
    array('data' => t('#')),
    array('data' => t('URL'), 'field' => 'visitors_url'),
    array('data' => t('Count'), 'field' => 'count', 'sort' => 'desc'),
  );

  $sql = sprintf("SELECT COUNT(visitors_id) AS count,
                  visitors_path,
                  MIN(visitors_title) AS visitors_title,
                  MIN(visitors_url) AS visitors_url
                  FROM {visitors}
                  WHERE %s
                  GROUP BY visitors_path" . tablesort_sql($header),
                  visitors_date_filter_sql_condition()
                );

  $sql_count = sprintf('SELECT COUNT(DISTINCT visitors_path)
                        FROM {visitors}
                        WHERE %s',
                        visitors_date_filter_sql_condition()
                      );

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

  $output  = visitors_date_filter();
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

  $from = visitors_get_from_timestamp();
  $to   = visitors_get_to_timestamp();

  $sql = sprintf('SELECT u.name,
                         u.uid,
                         count(DISTINCT v.visitors_id) as hits,
                         count(DISTINCT n.nid) as nodes,
                         count(DISTINCT c.cid) as comments
                  FROM {users} as u
                  LEFT JOIN {visitors} as v
                  ON u.uid=v.visitors_uid
                  LEFT JOIN {node} as n
                  ON u.uid=n.uid
                  AND n.created BETWEEN %s AND %s
                  LEFT JOIN {comments} as c
                  ON u.uid=c.uid
                  AND c.timestamp BETWEEN %s AND %s
                  WHERE %s
                  GROUP BY u.name, u.uid, v.visitors_uid, n.uid, c.uid
                  %s',
                  $from,
                  $to,
                  $from,
                  $to,
                  visitors_date_filter_sql_condition(),
                  tablesort_sql($header)
                );

  $sql_count = sprintf('SELECT COUNT(DISTINCT u.uid)
                        FROM {users} u
                        INNER JOIN {visitors} v
                        ON u.uid=v.visitors_uid
                        WHERE %s',
                        visitors_date_filter_sql_condition()
                      );
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
  $output  = visitors_date_filter();
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $items_per_page, 0);

  return $output;
}
