<?php

/**
 * @file
 * Contains all the forms.
 */

/**
 * Contains the line chart form.
 */
function instagram_line_chart_form($form, $form_state) {
  $form = [];
  $format = 'd-m-Y';
  $from = date('Y-m-d H:i:s', strtotime('7 days ago'));
  $to = date('Y-m-d H:i:s');

  $form['date_from'] = [
    '#type'                => 'date_popup',
    '#title'               => t('Show from:'),
    '#date_format'         => $format,
    '#date_label_position' => 'invisible',
    '#date_year_range'     => '-3:+3',
    '#default_value'       => $from,
    '#ajax'                => [
      'callback' => 'instagram_line_chart_ajax_callback',
    ],
  ];

  $form['date_to'] = [
    '#type'                => 'date_popup',
    '#title'               => t('to:'),
    '#date_format'         => $format,
    '#date_label_position' => 'invisible',
    '#date_year_range'     => '-3:+3',
    '#default_value'       => $to,
    '#ajax'                => [
      'callback' => 'instagram_line_chart_ajax_callback',
    ],
  ];
  $instagram_settings = _instagram_get_chart_settings(strtotime($from), strtotime($to));
  $form['line_chart'] = [
    '#type' => 'item',
    '#markup' => '<div id="line_chart"></div>',
  ];
  $form['#attached']['js'][] = [
    'data' => '//www.google.com/jsapi',
    'type' => 'external',
  ];
  $form['#attached']['js'][] = [
    'data' => '//www.gstatic.com/charts/loader.js',
    'type' => 'external',
  ];
  $form['#attached']['js'][] = [
    'data' => [
      'instagram_stats' => $instagram_settings,
    ],
    'type' => 'setting',
  ];
  $form['#attached']['js'][] = drupal_get_path('module', 'instagram') . '/js/chart.js';

  return $form;
}

/**
 * Ajax callback for the select from -- to.
 */
function instagram_line_chart_ajax_callback($form, &$form_state, $form_id) {

}

/**
 * Gets the stats between a certain date.
 */
function _instagram_get_follower_stats_range($from, $to) {
  $account_id = instagram_get_account_id();
  $select_stats = db_select('instagram', 'insta')
    ->fields('insta')
    ->condition('timestamp', $from, '>')
    ->condition('timestamp', $to, '<')
    ->condition('account_id', $account_id)
    ->execute()->fetchAll();
  // @TODO: Filter the account id.
  $stats_per_day = [];
  // We loop over the stats and group them by day. If there are multiple stats
  // on 1 day, we get the latest one that day.
  foreach ($select_stats as $stat) {
    $stats_per_day[date('Y-m-d', $stat->timestamp)] = [
      'followers' => $stat->followers_count,
      'following' => $stat->following_count,
      'count' => $stat->media_posted,
    ];
  }
  return $stats_per_day;
}

/**
 * Gets the settings.
 */
function _instagram_get_chart_settings($from, $to) {
  $instagram_stats = _instagram_get_follower_stats_range($from, $to);

  $header = array_keys($instagram_stats);
  $rows = $followers = $following = [];
  $columns = [
    'followers',
    'following',
  ];

  foreach ($instagram_stats as $date => $stat) {
    $followers[] = $stat['followers'];
    $following[] = $stat['following'];
  }

  $rows = [$followers, $following];
  $informazout_prices_chart_settings = [
    'header'      => $header,
    'rows'        => $rows,
    'columns'     => $columns,
    'chartType'   => 'LineChart',
    'containerId' => 'line_chart',
    'options'     => [
      'curveType'   => 'none',
      'forceIFrame' => FALSE,
      'title'       => '',
      'width' => 800,
      'height' => 400,
      'areaOpacity' => 0,
      'animation' => [
        'duration' => 1000,
        'easing' => 'in',
      ],
    ],
  ];

  return $informazout_prices_chart_settings;
}
