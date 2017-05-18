<!-- Download intro block -->
<?php $block = module_invoke('block', 'block_view', '63');
			print render($block['content']); ?>

<!-- Download button -->
<div>
<div class="btn-group">
  <button type="button" class="btn btn-primary btn-lg" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Download CiviCRM <?php echo $content['civicrm_version']; ?>&nbsp;&nbsp;
    <span class="caret"></span>
    <span class="sr-only">Toggle Dropdown</span>
  </button>
  <ul class="dropdown-menu">
	  <?php foreach ($content['download_urls'] as $key => $values) {
		  	if (!in_array('4.7', $values['versions'])) continue; 
		  	?>
	  		<li><?php
        $url = $values['url'];
        if (arg(1) == 'list' && variable_get('cividownload_mode') == 2 ) {
          $url = "https://download.civicrm.org/civicrm-{$content['civicrm_version']}-" . $values['filename'];
        }
        ?>
        <a href="<?php echo $url;?>" target="_blank">Download CiviCRM <?php echo $content['civicrm_version']; ?> <?php echo $values['title']; ?></a></li>
    <?php } ?>
  </ul>
  </div>
</div>
    <p>CiviCRM <?php echo $content['civicrm_version']; ?> is the latest version, containing the latest features and improvements. <a href="https://civicrm.org/blog/tags/release">View recent release announcements for details</a>.</p>
    <h2 class="thin border">Other options</h2>
    <h3 class="thick capital">Long term support</h3>
    <p>The current Long Term Support (LTS) release CiviCRM is <?php echo $content['civicrm_lts_version']; ?>. Select a download based on the content management software (CMS) you are using.</p>
    <ul>
  <?php foreach ($content['download_urls'] as $key => $values) { 
	  	  if (!in_array('4.6', $values['versions'])) continue; 
				?>
	  		<li><?php
        $url = $values['url'];
        if (arg(1) == 'list' && variable_get('cividownload_mode') == 2 ) {
          $url = "https://download.civicrm.org/civicrm-{$content['civicrm_lts_version']}-" . $values['filename'];
        }
        ?>
        <a href="<?php echo $url;?>" target="_blank">Download CiviCRM <?php echo $content['civicrm_lts_version']; ?> <?php echo $values['title']; ?></a></li>
    <?php } ?>
    </ul>

<!-- Download footer block -->  
<?php $block = module_invoke('block', 'block_view', '64');
			print render($block['content']); ?>	
<!-- Partner view -->
<?php echo views_embed_view('partners_and_contributors', 'block_4'); ?>