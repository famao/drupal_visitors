<?php

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
					
	$sql_count = db_rewrite_sql('SELECT COUNT(*) FROM {visitors}');

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
			check_plain($data->visitors_title).'<br>'.l($data->visitors_path, $data->visitors_url),
			$user_page,
			'<a href="hits/'.$data->visitors_id.'">'.t('details').'</a>'
		);
	}

	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}
function visitors_host_hits($ip)
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
				
	$sql = sprintf("SELECT v.*, u.name, u.uid 
                    FROM {visitors} v 
                    LEFT JOIN {users} u 
                    ON u.uid = v.visitors_uid 
                    WHERE v.visitors_ip=%d". tablesort_sql($header),
                    ip2long($ip));

	$count_sql = sprintf('SELECT COUNT(*) FROM {visitors} WHERE visitors_ip=%d', ip2long($ip));
	
	$sql_count = db_rewrite_sql($count_sql);

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
			check_plain($data->visitors_title).'<br>'.l($data->visitors_path, $data->visitors_url),
			$user_page,
			'<a href="../hits/'.$data->visitors_id.'">'.t('details').'</a>'
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
	
	$sql_count = db_rewrite_sql("SELECT count( DISTINCT visitors_ip ) FROM {visitors}");
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
            '<a href="hosts/'.long2ip($data->visitors_ip).'">'.t('hits').'</a>'

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
	
	$sql       = "select count(*) count, 
						date_format(from_unixtime(visitors_date_time), '%H') hour 
					from {visitors} 
					group by hour". tablesort_sql($header);
	
	$sql_count = db_rewrite_sql("SELECT count( DISTINCT date_format(from_unixtime(visitors_date_time), '%H') ) FROM {visitors}");
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
	
	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function visitors_days_of_week()
{
	$items_per_page = variable_get('default_nodes_main', 10);
	
	$header = array(t('#'), t('Day'),t('Pages'));
	
	$sql       = "select count(*) count, 
						date_format(from_unixtime(visitors_date_time), '%a') day,
						date_format(from_unixtime(visitors_date_time), '%w') num
					from {visitors} 
					group by day order by date_format(from_unixtime(visitors_date_time), '%w')";
	
	$sql_count = db_rewrite_sql("SELECT count( DISTINCT date_format(from_unixtime(visitors_date_time), '%a') ) FROM {visitors}");
	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$tmp_rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	
	while ( $data = db_fetch_object($results) ) 
	{
		$tmp_rows[$data->num] = array(
			$data->day,
			$data->count,
			$data->num
			);
	}
	
	$rows = array(
				array(1, 'Sun', 0),
				array(2, 'Mon', 0),
				array(3, 'Tue', 0),
				array(4, 'Wed', 0),
				array(5, 'Thu', 0),
				array(6, 'Fri', 0),
				array(7, 'Sat', 0),
				
					);
	
	foreach ($tmp_rows as $key => $tmp_item)
	{
		$rows[$key][2] = $tmp_item[1];
	}

	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
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
	
	$sql_count = db_rewrite_sql("SELECT count( DISTINCT date_format(from_unixtime(visitors_date_time), '%e') ) FROM {visitors}");
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
	
	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

function visitors_monthly_history()
{
	$items_per_page = variable_get('default_nodes_main', 10);
	
	$header = array(
					array('data' => t('#')),
					array('data' => t('Month'), 'field' => 'hour', 'sort' => 'asc'), 
					array('data' => t('Pages'), 'field' => 'count'), 
					);
	
	$sql       = "select count(*) count, 
						date_format(from_unixtime(visitors_date_time), '%Y %M') hour 
					from {visitors} 
					group by hour". tablesort_sql($header);
	
	$sql_count = db_rewrite_sql("SELECT count( DISTINCT date_format(from_unixtime(visitors_date_time), '%Y %M') ) FROM {visitors}");
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
	
	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}

/**
 * Menu callback; Displays recent page accesses.
 */
function visitors_hit_details($aid)
{
	$result = db_query('SELECT a.*, u.name, u.uid FROM {visitors} a LEFT JOIN {users} u ON a.visitors_uid = u.uid WHERE visitors_id = %d', $aid);
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
					
	$sql       = "SELECT *, count(*) count FROM {visitors} group by visitors_path". tablesort_sql($header);
	$sql_count = db_rewrite_sql('SELECT COUNT(DISTINCT visitors_path) FROM {visitors}');

	$results = pager_query($sql, $items_per_page, 0, $sql_count);

	$rows = array();

	$page = isset($_GET['page']) ? $_GET['page'] : '';
	$i = 0 + ($page  * $items_per_page);

	

	while ( $data = db_fetch_object($results) ) 
	{
		$rows[] = array(
			++$i,
            check_plain($data->visitors_title).'<br>'.l($data->visitors_path, $data->visitors_url),
			$data->count,
		);
	}

	$output  = "";
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, $items_per_page, 0);

	return $output;
}
