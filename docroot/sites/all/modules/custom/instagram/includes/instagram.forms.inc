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
  $instagram_chart_settings = [
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

  return $instagram_chart_settings;
}

/**
 * Creates line chart with the data 36 hours after the post.
 */
function instagram_hourly_media_chart($form, &$form_state, $media_id, $post_time) {
  $account_id = instagram_get_account_id();

  // We collect data until 36 hours after the post.
  $until_time = $post_time + (36 * 3600);

  $select_media_data = db_select('instagram_media', 'im')
    ->fields('im', ['timestamp', 'likes_count', 'video_views', 'comment_count'])
    ->condition('account_id', $account_id)
    ->condition('media_id', $media_id)
    ->condition('timestamp', $until_time, '<')
    ->execute()->fetchAll();

  $instagram_stats = [];
  foreach ($select_media_data as $media_data_item) {
    $hours_after_post = _instagram_get_hours_after_post($post_time, $media_data_item->timestamp);
    $instagram_stats[(string) $hours_after_post] = [
      'likes_count' => $media_data_item->likes_count,
      'video_views' => $media_data_item->video_views,
    ];
  }
  $settings = _instagram_get_hourly_chart_settings($instagram_stats, $media_id);

  $form['line_chart'] = [
    '#type' => 'item',
    '#markup' => '<div id="line_chart_' . $media_id . '"></div>',
  ];
//  $form['#attached']['js'][] = [
//    'data' => '//www.google.com/jsapi',
//    'type' => 'external',
//  ];
//  $form['#attached']['js'][] = [
//    'data' => '//www.gstatic.com/charts/loader.js',
//    'type' => 'external',
//  ];
  $form['#attached']['js'][] = [
    'data' => [
      'instagram_hourly_stats' => [
        $media_id => $settings,
      ],
    ],
    'type' => 'setting',
  ];
  $form['#attached']['js'][] = drupal_get_path('module', 'instagram') . '/js/hourly_chart.js';
  return $form;
}

/**
 * Gets the amount of hours after the post.
 */
function _instagram_get_hours_after_post($post_time, $data_time) {
  $difference_time = ($data_time - $post_time) / 3600;
  $difference_time = round($difference_time * 4) / 4;
  return $difference_time;
}

/**
 * Gets the hourly chart settings.
 */
function _instagram_get_hourly_chart_settings($instagram_stats, $media_id) {
//  $instagram_stats = _instagram_get_follower_stats_range($from, $to);

  $header = array_keys($instagram_stats);
  $rows = $likes = $views = [];
  $columns = [
    'likes',
    'views (videos)',
  ];

  foreach ($instagram_stats as $date => $stat) {
    $likes[] = $stat['likes_count'];
    $views[] = $stat['video_views'];
  }
  $rows = [$likes, $views];
  $instagram_chart_settings = [
    'header'      => $header,
    'rows'        => $rows,
    'columns'     => $columns,
    'chartType'   => 'LineChart',
    'containerId' => 'line_chart_' . $media_id,
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

  return $instagram_chart_settings;
}