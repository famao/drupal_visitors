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
require_once dirname(__FILE__) .'/reports/top_pages.php';
require_once dirname(__FILE__) .'/reports/user_activity.php';

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

