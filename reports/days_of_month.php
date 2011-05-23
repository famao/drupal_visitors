<?php

function visitors_days_of_month_data($header) {
  $query = db_select('visitors', 'v');
  $query->addExpression('COUNT(*)', 'count');
  $query->addExpression(visitors_date_format_sql('visitors_date_time', '%e'), 'day');
  $query->groupBy('day');
  visitors_date_filter_sql_condition($query);

  if (!is_null($header))
    $query->extend('TableSort')->orderByHeader($header);

  return $query->execute();
}

function visitors_days_of_month() {
  $header = array(
    array('data' => t('#')),
    array('data' => t('Day'), 'field' => 'day', 'sort' => 'asc'),
    array('data' => t('Pages'), 'field' => 'count'),
  );

  $results = visitors_days_of_month_data($header);
  $rows    = array();
  $i       = 0;
  $count   = 0;

  foreach ($results as $data) {
    $rows[] = array(
      ++$i,
      (int) $data->day,
      $data->count
    );

    $count += $data->count;
  }
  $output  = visitors_date_filter();

  if ($count > 0) {
    /* TODO: Add to img width and height. */
    $output .= '<img src="'. url('visitors/days_of_month/chart') .'" alt="'.t('Days of month').'">';
  }
  $output .= theme('table', array('header' => $header, 'rows' => $rows));

  return $output;
}

function chart_visitors_days_of_month() {
  $results = visitors_days_of_month_data(NULL);
  $rows = array();

  for ($i = 1; $i <= 31; $i++) {
    $rows[$i] = 0;
  }

  foreach ($results as $data) {
    $rows[(int)$data->day] = (int)$data->count;
  }

  // build dates series
  $dates = range(1, 31);
  visitors_chart($rows, $dates);
}

