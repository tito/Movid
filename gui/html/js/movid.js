/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/

// guess ourself the base url, needed for xhr
var mo_baseurl = location.href.split('/', 3).join('/');
var mo_available_inputs = [];
var mo_available_outputs = [];
var mo_streamscale = 2;
var mo_widget_selected = null;
var mo_status_text = 'stopped';
var mo_uniqidx = 0;
var mo_data = null;

function mo_uniq() {
	mo_uniqidx += 1;
	return 'mo' + mo_uniqidx;
}

function mo_resize() {
	$('#movidcanvas').attr('width', 10);
	$('#movidcanvas').attr('height', 10);

	// hardcoded...
	var h = $(window).height() - 100;
	var w = $(window).width() - 82;
	w -= $('#leftcolumn').width();
	w -= $('#rightcolumn').width();
	$('#movidcanvas').attr('width', w);
	$('#movidcanvas').attr('height', h);
	if ( widgetCanvasResize != null )
		widgetCanvasResize(w, h);
}

function mo_bootstrap() {
	$('#container-preview').hide();
	$('#container-properties').hide();
	$('#btn-create').addClass('ui-state-active');

	$(window).resize(mo_resize);

	// prepare buttons (hover + actions)
	$(function(){
		//all hover and click logic for buttons
		$('.fg-button:not(.ui-state-disabled,.ui-fake)')
		.hover(
			function(){
				$(this).addClass('ui-state-hover');
			},
			function(){
				$(this).removeClass('ui-state-hover');
			}
		)
		.mousedown(function(){
			switch ($(this).attr('id')) {
				case 'btn-start':
					if ( $(this).hasClass('ui-state-active') )
						mo_stop();
					else
						mo_start();
					break;
				case 'btn-create':
				case 'btn-properties':
				case 'btn-preview':
					var container = '#container-' + $(this).attr('id').split('-')[1];
					if ( $(this).hasClass('ui-state-active') ) {
						$(container).hide();
						$(this).removeClass('ui-state-active');
					} else {
						$(container).show();
						$(this).addClass('ui-state-active');
					}
					mo_resize();
					break;

				default:
					break;
			}
		});

	});

	Processing($('#movidcanvas')[0], $('#movidpjs')[0].text);

	setTimeout(mo_resize, 50);
	mo_modules();
	mo_status();
	mo_stats();
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
		$('#btn-start').removeClass('ui-state-active');
		if ( data['status']['running'] != '0' )
			$('#btn-start').addClass('ui-state-active');
		$('#version').html(data['status']['version']);

		widgetClearConnectivity();

		for ( key in data['status']['modules'] ) {
			var infos = data['status']['modules'][key];
			if ( widgetGet(key) == null ) {
				var _x = infos['properties']['x'];
				var _y = infos['properties']['y'];
				var _gui = infos['gui'];
				if ( typeof _x == 'undefined' )
					_x = 0;
				if ( typeof _y == 'undefined' )
					_y = 0;
				widgetCreate(key, _gui);
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
	});
}

function mo_create(elem) {
	$.get(mo_baseurl + '/pipeline/create?objectname=' + elem, function(data) {
		mo_status();
		mo_select(data['message']);
	});
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

	// ask for the status of the pipeline,
	// filter on the UI we want, and contruct properties list.
	$.get(mo_baseurl + '/pipeline/status', function(data) {
		for ( key in data['status']['modules'] ) {
			if ( key != elem )
				continue;

			// extract info about our module
			infos = data['status']['modules'][key];

			// all elements will be in a table, prepare it
			var table = $('<table></table>');
			if ( infos['gui'] == 1 ) {
				var uniq = mo_uniq();
				var input =
					$('<input></input>')
					.attr('id', uniq)
					.attr('type', 'checkbox')
					.attr('onchange', 'javascript:mo_gui("'
						+ elem + '", 0)');
				var tr = $('<tr></tr>')
					.append( $('<td></td>')
						.addClass('label')
						.html('GUI')
					)
					.append( $('<td></td>')
						.append(input)
						.append(
							$('<label></label>')
							.attr('for', uniq)
							.html('Open'))
					);
				table.append(tr);
			}


			// enumerate properties
			for ( var property in infos['properties'] ) {
				var tr = $('<tr></tr>');
				var td = $('<td></td>');

				// add the label into the table
				value = infos['properties'][property];
				tr.append($('<td></td>')
					.addClass('label')
					.html(property)
				);

				// extract properties infos
				pinfo = infos['propertiesInfos'][property];

				//
				// bool
				//
				if ( pinfo['type'] == 'bool' ) {
					var uniq = mo_uniq();
					var input =
						$('<input></input>')
						.attr('id', uniq)
						.attr('type', 'checkbox')
						.attr('checked', value=='true'?'checked':'')
						.attr('onchange', 'javascript:mo_set("'
							+ elem + '", "' + property
							+ '", this.checked ? "true" : "false")');
					td.append(input);
					td.append(
						$('<label></label>')
						.attr('for', uniq)
						.html('Activate')
					);


				//
				// double
				//
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
					td.append(slider);

				//
				// choice list, use a select
				//
				} else if ( typeof pinfo['choices'] != 'undefined' ) {
					var s = $('<select></select>')
						.addClass('ui-widget ui-widget-content')
						.attr('onchange', 'javascript:mo_set("'
							+ elem + '", "' + property
							+ '", this.value)')
					var choices = pinfo['choices'].split(';');
					for ( var i = 0; i < choices.length; i++ ) {
						choice = choices[i];
						s.append($('<option></option>')
							.attr('value', choice)
							.attr('selected', value == choice?'selected':'')
							.html(choice)
						);
					}
					td.append(s);

				//
				// default case, use a simple input
				//
				} else {
					td.append(
						$('<input></input>')
						.addClass('text ui-widget-content')
						.attr('type', 'text')
						.attr('value', value)
						.attr('onblur', 'javascript:mo_set("'
							+ elem + '", "' + property
							+ '", this.value)')
					);
				}

				// add the property to the table
				tr.append(td);
				table.append(tr);
			}

			// show table !
			$('#properties').html(table);
		}
	});

	// slide slide :)
	$('#properties').slideDown('fast');

	// WHYYYYYYYYYYYY ? :' :' :(
	setTimeout(_mo_update_state, 20);
}

function _mo_update_state() {
	$('#properties input[type=\"checkbox\"]').button();
}

function mo_set(id, k, v, callback) {
	$.get(mo_baseurl + '/pipeline/set?objectname=' + id + '&name=' + k + '&value=' + v, function(data) {
		if ( typeof(callback) != 'undefined' )
			callback();
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
	mo_widget_selected = elem;
	mo_properties(elem);
	mo_stream(elem);
}

function mo_gui(elem, is_update) {
	$.get(mo_baseurl + '/pipeline/gui?objectname=' + elem, function(data) {
		widgetConfigure(data, is_update);
	});
}

function mo_stats() {
	$.get(mo_baseurl + '/pipeline/stats', function(data) {
		var report = 'FPS: -';
		var count = 0,
			average_fps = 0,
			average_process_time = 0,
			average_wait_time = 0;
		for ( key in data.stats ) {
			count++;
			average_fps += parseFloat(data['stats'][key]['average_fps']);
			average_process_time += parseFloat(data['stats'][key]['average_process_time']);
			average_wait_time += parseFloat(data['stats'][key]['average_wait_time']);
		}
		// TODO, show process/wait time
		if ( mo_status_text == 'running' && count > 0 )
			report = 'FPS: ' + (average_fps / count).toFixed(2);
		$('#toolbareport').html(report);
		setTimeout(mo_stats, 2000);
	});
}

// once our document is loaded, load our software.
$(document).ready(function() {
	mo_bootstrap();
});
