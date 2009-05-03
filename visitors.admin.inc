<?php

// $Id$

require_once 'pChart/pData.class';
require_once 'pChart/pChart.class';

function visitors_graph($values, $x_array = NULL)
{
  // Dataset definition 
  $DataSet = new pData;
  $DataSet->AddPoint($values,"Serie1");
  
  if ($x_array !== NULL)
  {
    $DataSet->AddPoint($x_array,"Serie2");
    $DataSet->SetAbsciseLabelSerie("Serie2");
  }
  
  $DataSet->AddSerie("Serie1");
  $DataSet->SetSerieName("","Serie1");

  // Initialise the graph
  $Test = new pChart(700,430);
  $Test->setFontProperties(dirname(__FILE__)."/Fonts/tahoma.ttf",8);
  $Test->setGraphArea(50,30,680,400);
  $Test->drawFilledRoundedRectangle(7,7,693,423,5,240,240,240);
  $Test->drawRoundedRectangle(5,5,695,425,5,230,230,230);
  $Test->drawGraphArea(255,255,255,TRUE);
  $Test->setFixedScale(0,0,0,0,0,0,0);
  $Test->drawScale($DataSet->GetData(),$DataSet->GetDataDescription(),
                   SCALE_ADDALLSTART0,150,150,150,TRUE,0,0,TRUE
                  );
  $Test->drawGrid(4,TRUE,230,230,230,50);

  // Draw the 0 line
  $Test->drawTreshold(0,143,55,72,TRUE,TRUE);

  // Draw the bar graph
  $Test->drawBarGraph($DataSet->GetData(),$DataSet->GetDataDescription(),TRUE);

  // Finish the graph
  $Test->Stroke();
  exit();
}

function visitors_recent_hits()
{
	$date_format = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
	$items_per_page = variable_get('default_nodes_main', 10);
	
	$header = array(
					array('data' => t('#')),
					array('data' => t('ID'), 'field' => 'visitors_id', 'sort' => 'desc'), 
					array('data' => t('Date'), 'field' => 'visitors_date_time'), 
					array('data' => t('URL'), 'field' => 'visitors_url'), 
					array('data' => t('User'), 'field' => 'u.name'), 
					array('data' => t('Operations'))
					);
					
	$sql       = "SELECT v.*, u.name, u.uid 
					FROM {visitors} v 
					LEFT JOIN {users} u 
					ON u.uid = v.visitors_uid". tablesort_sql($header);
					
	$sql_count = 'SELECT COUNT(*) FROM {visitors}';

	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	while ( $data = db_fetch_object($results) ) 
	{
		$user = user_load(array('uid' => $data->visitors_uid));
		$user_page = theme('username', $data);
		$rows[] = array(
			++$i,
			$data->visitors_id,
			date($date_format, $data->visitors_date_time),
			check_plain($data->visitors_title).'<br>'.
			  l($data->visitors_path, $data->visitors_url),
			$user_page,
			l(t('details'), 'admin/visitors/hits/'.$data->visitors_id)
		);
	}

	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function visitors_host_hits($ip)
{
  if (!visitors_is_ip_valid($ip))
  {
    return drupal_not_found();
  }
  
	$date_format = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
	$items_per_page = variable_get('default_nodes_main', 10);
	
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
                  WHERE v.visitors_ip=%d". tablesort_sql($header),
                  ip2long($ip));

	$count_sql = sprintf('SELECT COUNT(*) count 
	                      FROM {visitors} 
	                      WHERE visitors_ip=%d', 
	                      ip2long($ip)
	                    );
	                    
  $query = db_query($count_sql);
  $data = db_fetch_object($query);

  if ($data->count == 0)
  {
    return drupal_not_found();
  }

	$results = pager_query($sql, $items_per_page, 0, $count_sql);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	while ( $data = db_fetch_object($results) ) 
	{
		$user = user_load(array('uid' => $data->visitors_uid));
		$user_page = theme('username', $data);
		$rows[] = array(
			++$i,
			$data->visitors_id,
			date($date_format, $data->visitors_date_time),
			check_plain($data->visitors_title).'<br>'.
			  l($data->visitors_path, $data->visitors_url),
			$user_page,
			l(t('details'), 'admin/visitors/hits/'.$data->visitors_id)
		);
	}

	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);
	drupal_set_title(t('Hits from ') . check_plain($ip));
	
	return $output;
}

function visitors_hosts()
{
	$items_per_page = variable_get('default_nodes_main', 10);
	
	$header = array(
					array('data' => t('#')),
					array('data' => t('Host'), 'field' => 'visitors_ip'), 
					array('data' => t('Pages'), 'field' => 'count', 'sort' => 'desc'),
	                array('data' => t('Operations'))
					);
	
	$sql       = "SELECT count( * ) as count, visitors_ip
					FROM {visitors}
					GROUP BY visitors_ip". tablesort_sql($header);
	
	$sql_count = "SELECT count( DISTINCT visitors_ip ) FROM {visitors}";
	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	
	while ( $data = db_fetch_object($results) ) 
	{
		$rows[] = array(
			++$i,
			long2ip($data->visitors_ip),
			$data->count,
      l(t('hits'), 'admin/visitors/hosts/'.long2ip($data->visitors_ip))
		);
	}
	
	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function visitors_hours()
{
	$items_per_page = 24;
	
	$header = array(
					array('data' => t('#')),
					array('data' => t('Hour'), 'field' => 'hour', 'sort' => 'asc'), 
					array('data' => t('Pages'), 'field' => 'count'), 
					);
	
  $sql = "select count(*) count, 
          date_format(from_unixtime(visitors_date_time), '%H') hour 
          from {visitors} 
          group by hour". tablesort_sql($header);
	
	$sql_count = "SELECT count( 
	              DISTINCT date_format(from_unixtime(visitors_date_time), '%H')) 
	              FROM {visitors}";
	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	
	while ( $data = db_fetch_object($results) ) 
	{
		$rows[] = array(
			++$i,
			
			$data->hour,
			$data->count
			);
	}
	
	$output  = "<img src='".url()."admin/visitors/hours/graph'>";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function graph_visitors_hours()
{
  $sql = "select count(*) count, 
          date_format(from_unixtime(visitors_date_time), '%H') hour 
          from {visitors} 
          group by hour";
          
  $result = db_query($sql);
	$tmp_rows = array();
	$rows = array();
	for ($i = 0; $i < 24; $i++)
	{
	  $rows[$i] = 0;
	}
	
	while ( $data = db_fetch_object($result) ) 
	{
	  $rows[(int)$data->hour] = $data->count;
	}
	
  $hours = range(0, 23);

  visitors_graph($rows, $hours);
}

function visitors_days_of_week()
{
	$header = array(t('#'), t('Day'),t('Pages'));
	
	$sql = "select count(*) count, 
					date_format(from_unixtime(visitors_date_time), '%a') day,
					date_format(from_unixtime(visitors_date_time), '%w') num
				  from {visitors} 
				  group by day 
				  order by date_format(from_unixtime(visitors_date_time), '%w')";
	
	$result = db_query($sql);
	$tmp_rows = array();

	while ( $data = db_fetch_object($result) ) 
	{
		$tmp_rows[$data->num] = array(
			$data->day,
			$data->count,
			$data->num
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
	
	foreach ($tmp_rows as $key => $tmp_item)
	{
		$rows[$key][2] = $tmp_item[1];
	}
	
	$output  = "<img src='".url()."admin/visitors/days_of_week/graph'>";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function graph_visitors_days_of_week()
{
  $width = 800;
  $height = 300;
  
  $graph['type'] = 'bars';
  
  $graph['title'] = '';
  
  $graph['xlabel'] = 'day';
  $graph['ylabel'] = 'pages';
  
  $sql = "select count(*) count, 
					date_format(from_unixtime(visitors_date_time), '%a') day,
					date_format(from_unixtime(visitors_date_time), '%w') num
				  from {visitors} 
				  group by day 
				  order by date_format(from_unixtime(visitors_date_time), '%w')";
  $result = db_query($sql);
	$tmp_rows = array();

	while ( $data = db_fetch_object($result) ) 
	{
		$tmp_rows[$data->num] = array(
			$data->day,
			$data->count,
			$data->num
			);
	}

	foreach ($tmp_rows as $key => $tmp_item)
	{
		$rows[] = $tmp_item[1];
	}

 // build dates series
	$dates = array(t('Sun'), t('Mon'), t('Tue'), t('Wed'), 
	               t('Thu'), t('Fri'), t('Sat')
	              );
	
  visitors_graph($rows, $dates);
}

function visitors_days_of_month()
{
	$items_per_page = 31;
	
	$header = array(
					array('data' => t('#')),
					array('data' => t('Day'), 'field' => 'hour', 'sort' => 'asc'), 
					array('data' => t('Pages'), 'field' => 'count'), 
					);
	
	$sql       = "select count(*) count, 
						date_format(from_unixtime(visitors_date_time), '%e')+0 hour 
					from {visitors} 
					group by hour". tablesort_sql($header);
	
	$sql_count = "SELECT count(DISTINCT date_format(from_unixtime(visitors_date_time), '%e')) FROM {visitors}";
	              
	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	
	while ( $data = db_fetch_object($results) ) 
	{
		$rows[] = array(
			++$i,
			
			$data->hour,
			$data->count
			);
	}
	
	$output  = "<img src='".url()."admin/visitors/days_of_month/graph'>";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function graph_visitors_days_of_month()
{
  $width = 800;
  $height = 300;
  
  $graph['type'] = 'bars';
  
  $graph['title'] = '';
  
  $graph['xlabel'] = 'day';
  $graph['ylabel'] = 'pages';
  
  $sql       = "select count(*) count, 
						date_format(from_unixtime(visitors_date_time), '%e')+0 hour 
					from {visitors} 
					group by hour";
  $results = pager_query($sql, 31, 0, NULL);
  $rows = array();
  
  for ($i = 1; $i <= 31; $i++)
  {
    $rows[$i] = 0;
  }
  
	while ( $data = db_fetch_object($results) ) 
	{
		$rows[(int)$data->hour] = (int)$data->count;
	}		
	
	// build dates series
	$dates = range(1, 31);
	
  visitors_graph($rows, $dates);
}

function visitors_monthly_history()
{
	$items_per_page = variable_get('default_nodes_main', 10);
	
	$header = array(
					array('data' => t('#')),
					array('data' => t('Month'), 'field' => 'visitors_date_time', 'sort' => 'asc'), 
					array('data' => t('Pages'), 'field' => 'count'), 
					);
	
	$sql       = "select count(*) count, 
						date_format(from_unixtime(visitors_date_time), '%Y %M') month 
					from {visitors} 
					group by month". tablesort_sql($header);
	
	$sql_count = "SELECT count(DISTINCT date_format(from_unixtime(visitors_date_time), '%Y %M')) FROM {visitors}";
	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	
	while ( $data = db_fetch_object($results) ) 
	{
		$rows[] = array(
			++$i,
			
			$data->month,
			$data->count
			);
	}
	
	$output  = "<img src='".url()."admin/visitors/monthly_history/graph'>";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function graph_visitors_monthly_history()
{
	
	$sql = "select count(*) count, 
          date_format(from_unixtime(visitors_date_time), '%Y %M') month 
          from {visitors} 
          group by month
          order by visitors_date_time asc";
          
	$rows = array();
  $dates = array();
  
  $result = db_query($sql);
  
	while ( $data = db_fetch_object($result) ) 
	{
    $rows[$data->month] = $data->count;
	  $dates[] = $data->month;
	}
	
	visitors_graph($rows, $dates);
}

/**
 * Menu callback; Displays recent page accesses.
 */
function visitors_hit_details($aid)
{
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

function visitors_top_pages()
{
	$date_format = variable_get('date_format_short_custom', 'Y-m-d H:i:s');
	$items_per_page = variable_get('default_nodes_main', 10);
	
	$header = array(
					array('data' => t('#')), 
					array('data' => t('URL'), 'field' => 'visitors_url'), 
					array('data' => t('count'), 'field' => 'count', 'sort' => 'desc'), 
					);
					
	$sql       = "SELECT *, count(*) count 
	              FROM {visitors} 
	              group by visitors_path". tablesort_sql($header);
	$sql_count = 'SELECT COUNT(DISTINCT visitors_path) FROM {visitors}';

	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	while ($data = db_fetch_object($results)) {
		$rows[] = array(
			++$i,
      check_plain($data->visitors_title).'<br>'.
      l($data->visitors_path, $data->visitors_url),
			$data->count,
		);
	}

	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}
