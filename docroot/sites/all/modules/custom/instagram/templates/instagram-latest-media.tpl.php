<?php

/**
 * @file
 * Displays the latest media.
 */
?>
<div class="instagram-latest-media">
  <?php foreach($media as $item): ?>
    <div class="instagram-latest-media-item">
      <div class="instagram-latest-media-item-left">
        <img src="<?php print $item['thumbnail']; ?>" width="150px" height="auto"/>
      </div>
      <div class="instagram-latest-media-item-right">
        <div class="instagram-latest-media-item-likes"><div class="label"><?php print t('Likes:'); ?></div> <?php print $item['likes']->count; ?></div>
        <div class="instagram-latest-media-item-comments"><div class="label"><?php print t('Comments:'); ?></div> <?php print $item['comments']->count; ?></div>
        <div class="instagram-latest-media-item-caption"><div class="label"><?php print t('Caption:'); ?></div> <?php print $item['caption']; ?></div>
      </div>
        <div class="instagram-latest-media-item--graph">
            <?php print $item['chart']; ?>
        </div>
      </div>
  <?php endforeach; ?>
</div>
