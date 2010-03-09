var mo_baseurl = 'http://127.0.0.1:7500';
var mo_available_inputs = [];
var mo_available_outputs = [];
var mo_streamscale = 2;
var mo_widget_selected = null;
var mo_status_text = 'stopped';

function mo_bootstrap() {
	$('#b_start').hide();
	$('#b_stop').hide();
	$('#modules').toggle();
	$('#video').toggle();
	$('#properties').toggle();

	Processing($('#movidcanvas')[0], $('#movidpjs')[0].text);

	mo_modules();
	mo_status();
}

function mo_modules() {
	$.get(mo_baseurl + '/factory/list', function(data) {
		$('#modules').html('');
		$(data['list']).each(function (index, elem) {
			$('#modules').append(
				$('<a></a>')
				.html(elem)
				.addClass('module')
				.attr('href', 'javascript:mo_create("' + elem + '")')
			);
		});
	});
}

function mo_status() {
	$.get(mo_baseurl + '/pipeline/status', function(data) {
		mo_available_inputs = [];
		mo_available_outputs = [];

		mo_status_text = data['status']['running'] == '0' ? 'stopped' : 'running'
		$('#statusinfo').html(mo_status_text);

		if ( mo_status_text == 'stopped' ) {
			$('#b_start').show();
			$('#b_stop').hide();
		} else {
			$('#b_start').hide();
			$('#b_stop').show();
		}


		widgetClearConnectivity();

		for ( key in data['status']['modules'] ) {
			var infos = data['status']['modules'][key];
			if ( widgetGet(key) == null ) {
				widgetCreate(key);
				var _x = infos['properties']['x'];
				var _y = infos['properties']['y'];
				if ( typeof _x == 'undefined' )
					_x = 0;
				if ( typeof _y == 'undefined' )
					_y = 0;
				widgetPosition(key, _x, _y);

				if ( typeof(infos['inputs']) != 'undefined' ) {
					for ( idx in infos['inputs'] ) {
						input = infos['inputs'][idx];
						widgetAddInput(key, input['name'], input['type']);
					}
				}
				if ( typeof(infos['outputs']) != 'undefined' ) {
					for ( idx in infos['outputs'] ) {
						input = infos['outputs'][idx];
						widgetAddOutput(key, input['name'], input['type']);
					}
				}
			}
		}

		for ( key in data['status']['modules'] ) {
			var infos = data['status']['modules'][key];
			if ( typeof(infos['outputs']) != 'undefined' ) {
				for ( idx in infos['outputs'] ) {
					var output = infos['outputs'][idx];
					for ( k in output['observers'] ) {
						widgetConnect(key, idx, output['observers'][k], 0);
					}
				}
			}
		}


			/**
			$('#instances_content').append(
				$('<a></a>')
				.html(key)
				.addClass('module')
				.attr('id', 'mod_' + key)
				.attr('href', 'javascript:mo_properties("' + key + '")')
			);

			for ( idx in infos['inputs'] ) {
				input = infos['inputs'][idx];
				mo_available_inputs[mo_available_inputs.length] = key + ':' + input['index'];
			}

			for ( idx in infos['outputs'] ) {
				output = infos['outputs'][idx];
				mo_available_outputs[mo_available_outputs.length] = key + ':' + output['index'];
			}
			**/
	});
}

function mo_create(elem) {
	$.get(mo_baseurl + '/pipeline/create?objectname=' + elem, function(data) {
		mo_status();
		mo_select(data['message']);
	});
	$('#modules').slideToggle('fast');
}

function mo_remove(elem) {
	$.get(mo_baseurl + '/pipeline/remove?objectname=' + elem, function(data) {
		mo_status();
		mo_select('');
	});
}

function mo_properties(elem) {
	if ( elem == '' ) {
		$('#properties').html('');
		$('#properties').slideUp('fast');
		mo_status();
		return;
	}

	$.get(mo_baseurl + '/pipeline/status', function(data) {
		for ( key in data['status']['modules'] ) {
			if ( key != elem )
				continue;
			infos = data['status']['modules'][key];

			// update properties
			$('#properties').html('');
			for ( var property in infos['properties'] ) {
				value = infos['properties'][property];
				$('#properties').append(
					$('<p></p>')
					.html(property)
				);
				pinfo = infos['propertiesInfos'][property];
				if ( pinfo['type'] == 'bool' ) {
					$('#properties').append(
						$('<select></select>').append(
							$('<option></option>')
							.attr('value', 'true')
							.attr('selected', value == 'true'?'selected':'')
							.html('True')
						).append(
							$('<option></option>')
							.attr('value', 'false')
							.attr('selected', value == 'false'?'selected':'')
							.html('False')
						).attr('onchange',
							'javascript:mo_set("' + elem + '", "' + property + '", this.value)')
					);
				} else if ( pinfo['type'] == 'double' ) {
					var slider = $('<div></div>').slider().slider('option', 'value', value);
					if ( typeof pinfo['min'] != 'undefined' )
						slider.slider('option', 'min', pinfo['min']);
					if ( typeof pinfo['max'] != 'undefined' )
						slider.slider('option', 'max', pinfo['max']);
					var _p = property;
					var _e = elem;
					slider.bind('slidechange', function(event, ui) {
							mo_set(_e, _p, ui.value);
					});
					$('#properties').append(slider);
				} else if ( typeof pinfo['choices'] != 'undefined' ) {
					var s = $('<select></select>')
						.attr('onchange', 'javascript:mo_set("' + elem + '", "' + property + '", this.value)')
					var choices = pinfo['choices'].split(';');
					for ( var i = 0; i < choices.length; i++ ) {
						choice = choices[i];
						s.append($('<option></option>')
							.attr('value', choice)
							.attr('selected', value == choice?'selected':'')
							.html(choice)
						);
					}
					$('#properties').append(s);
				} else {
					$('#properties').append(
						$('<input></input>')
						.attr('type', 'text')
						.attr('value', value)
						.attr('onblur', 'javascript:mo_set("' + elem + '", "' + property + '", this.value)')
					);
				}
			}
		}
	});

	$('#properties').slideDown('fast');
}

function mo_set(id, k, v) {
	$.get(mo_baseurl + '/pipeline/set?objectname=' + id + '&name=' + k + '&value=' + v, function(data) {
		// TODO
	});
}

function mo_connect(input, inidx, output, outidx) {

	$.get(mo_baseurl + '/pipeline/connect?in=' + input + '&out=' + output + '&inidx=' + inidx + '&outidx=' + outidx, function(data) {
	});
}

function mo_start() {
	$.get(mo_baseurl + '/pipeline/start', function(data) {
		mo_status();
	});
}

function mo_stop() {
	mo_stream('');
	$.get(mo_baseurl + '/pipeline/stop', function(data) {
		mo_status();
	});
}

function mo_stream(elem) {
	if ( mo_status_text == 'stopped' ) {
		$('#video').slideUp('fast');
		return;
	}
	if ( elem == '' ) {
		$('#streamid').html('No video');
		$('#streamimg').attr('src', '/gui/nostream.png');
		$('#video').slideUp('fast');
	} else {
		$('#streamid').html('Video of ' + elem);
		$('#streamimg').attr('src', mo_baseurl + '/pipeline/stream?objectname=' + elem + '&scale=' + mo_streamscale + '#' + Math.random() * 10000);
		$('#video').slideDown('fast');
	}
}

function mo_select(elem) {
	mo_properties(elem);
	mo_stream(elem);
}


$(document).ready(function() { mo_bootstrap(); });
