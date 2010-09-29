//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Copyright (C) 2010 Movid Authors.  All rights reserved.                  //
//                                                                          //
// This file is part of the Movid Software.                                 //
//                                                                          //
// This file may be distributed under the terms of the Q Public License     //
// as defined by Trolltech AS of Norway and appearing in the file           //
// LICENSE included in the packaging of this file.                          //
//                                                                          //
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE  //
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.//
//                                                                          //
// Contact info@movid.org if any conditions of this licensing are           //
// not clear to you.                                                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// Cookie expiration in 60 days
// days = 60;
// CookieExpires = new Date();
// CookieExpires.setTime( CookieExpires.getTime() + ( 60 * 60 * 1000 * 24 * days ) );
// $.cookies.setOptions({ expiresAt: CookieExpires });

// guess ourself the base url, needed for xhr
var mo_baseurl = location.href.split('/', 3).join('/');
var mo_available_modules = {};
var mo_available_inputs = [];
var mo_available_outputs = [];
var mo_streamscale = 2;
var mo_widget_selected = null;
var mo_status_text = 'stopped';
var mo_uniqidx = 0;
var mo_data = null;
var mo_host = null;
var mo_port = '7500';
var mo_slot_dialog = null;
var mo_slot_action = 'load';
var pjs = null;

function moModule(name) {
	this.name = name;
	this.inputs = [];
	this.outputs = [];
}

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

	var hostport = mo_baseurl.split('/');
	hostport = hostport[hostport.length - 1].split(':');
	if ( hostport.length == 1 ) {
		mo_host = hostport[0];
	} else {
		mo_host = hostport[0];
		mo_port = hostport[1];
	}

	// prepare selection
	$('#container-preview').hide();
	$('#btn-module').addClass('ui-state-active');
	$('#btn-properties').addClass('ui-state-active');
	$('#btn-create').addClass('ui-state-active');
	$('#pipeline-content-text').hide();
	$('#menu-pipeline').menu({
		content: $('#menu-pipeline-content').html(),
		flyOut: true,
		showSpeed: 0,
		crossSpeed: 0,
	})

	$('#menu-windows').menu({
		content: $('#menu-windows-content').html(),
		flyOut: true,
		showSpeed: 0,
		crossSpeed: 0,
	})

	$(window).resize(mo_resize);

	// prepare buttons (hover + actions)
	$('.mo-action')
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
				mo_start();
				break;
			case 'btn-stop':
				mo_stop();
				break;
			case 'btn-autolayout':
				mo_layout();
				break;
			case 'slot-1':
			case 'slot-2':
			case 'slot-3':
			case 'slot-4':
			case 'slot-5':
			case 'slot-6':
			case 'slot-7':
			case 'slot-8':
			case 'slot-9':
			case 'slot-10':
				var slotidx = $(this).attr('id').split('-')[1];
				mo_slot_dialog.dialog('close');
				if ( mo_slot_action == 'load' )
					mo_slot_load(slotidx);
				else
					mo_slot_save(slotidx);
				break;

			default:
				break;
		}
	});

	pjs = Processing($('#movidcanvas')[0], $('#movidpjs')[0].text);
	mo_pjs();

	setTimeout(mo_resize, 50);
	mo_modules();
	mo_status();
	mo_stats();
}

function mo_menu_select(item, itemid) {
	switch (itemid) {
		case 'pipeline-text':
			$(this).addClass('ui-state-active');
			$('#pipeline-content-textarea').width($('#pipeline-content-canvas').width());
			$('#pipeline-content-textarea').height($('#pipeline-content-canvas').height());
			$.get(mo_baseurl + '/pipeline/dump', function(data) {
				$('#pipeline-content-textarea').html(data);
				$('#pipeline-content-text').dialog({
					title: 'Pipeline text',
					modal: true,
					width: $('#pipeline-content-canvas').width() + 40,
					height: $('#pipeline-content-canvas').height() + 40,
				});
			});
			break;
		case 'pipeline-download':
			window.location = mo_baseurl + '/pipeline/dump?download=1';
			break;
		case 'pipeline-slot-load':
			mo_slot_refresh(function() {
				mo_slot_action = 'load';
				mo_slot_dialog = $('#pipeline-slot-content').dialog({
					title: 'Load pipeline from a slot',
					modal: true
				});
			});
			break;
		case 'pipeline-slot-save':
			mo_slot_refresh(function() {
				mo_slot_action = 'save';
				mo_slot_dialog = $('#pipeline-slot-content').dialog({
					title: 'Save pipeline to slot',
					modal: true
				});
			});
			break;
		case 'btn-module':
		case 'btn-create':
		case 'btn-properties':
		case 'btn-preview':
			var container = '#container-' + itemid.split('-')[1];
			if ( $(item).hasClass('ui-state-active') ) {
				$(container).hide();
				$(item).removeClass('ui-state-active');
			} else {
				$(container).show();
				$(item).addClass('ui-state-active');
			}
			mo_resize();
			break;

	}
}

function mo_modules() {
	$.get(mo_baseurl + '/factory/list', function(data) {
		$('#modules').html('');
		$(data['list']).each(function (index, elem) {
			$('#modules').append(
				$('<a></a>')
				.html(elem)
				.addClass('module')
				.attr('id', 'module_' + elem)
				.attr('href', 'javascript:mo_create("' + elem + '")')
			);
		});

		$.each(data['details'], function (name, infos) {
			var module = new moModule(name);
			$(infos['inputs']).each(function (index, elem) {
				module.inputs.push(elem);
			});
			$(infos['outputs']).each(function (index, elem) {
				module.outputs.push(elem);
			});
			mo_available_modules[name] = module;
		});
		mo_resize();
	});
}

function mo_menu_set_running(running) {
	if ( running ) {
		$('#btn-stop').removeClass('ui-state-active');
		$('#btn-start').addClass('ui-state-active');
	} else {
		$('#btn-stop').addClass('ui-state-active');
		$('#btn-start').removeClass('ui-state-active');
	}
}

function mo_status() {
	$.get(mo_baseurl + '/pipeline/status', function(data) {
		mo_available_inputs = [];
		mo_available_outputs = [];

		mo_status_text = data['status']['running'] == '0' ? 'stopped' : 'running'
		mo_menu_set_running(data['status']['running'] != '0');
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

function mo_set(id) {
	var callback = null;
	var url = mo_baseurl + '/pipeline/set?objectname=' + id;

	if ( typeof(arguments[arguments.length - 1]) == "function" )
		callback = arguments[arguments.length - 1];

	for ( var i = 1; i < arguments.length; i += 2 ) {
		url += '&name=' + arguments[i];
		url += '&value=' + arguments[i+1];
	}

	$.get(url, function(data) {
		if ( callback != null )
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
	mo_gui_cancel();
}

function mo_gui_cancel() {
	var base = document.getElementById('flashgui');
	if ( base )
		base.parentNode.removeChild(base);
}

function mo_gui(elem, is_update) {
	/**
	$.get(mo_baseurl + '/pipeline/gui?objectname=' + elem, function(data) {
		widgetConfigure(data, is_update);
	});
	**/

	// toggle GUI
	var base = document.getElementById('flashgui');
	if ( base ) {
		base.parentNode.removeChild(base);
		return;
	}

	// construct basics parameters
	var src = mo_baseurl + '/gui/flashgui.swf?objectname=' + elem;
	var flashvars = [
		'objectname=', encodeURIComponent(elem),
		'&ip=', mo_host,
		'&port=', mo_port
	].join('');

	// Internet explorer support
	base = document.createElement('OBJECT');
	base.setAttribute('id', 'flashgui');
	base.setAttribute('width', '640');
	base.setAttribute('height', '480');
	param = document.createElement('PARAM');
	param.setAttribute('name', 'movie');
	param.setAttribute('value', src);
	base.appendChild(param);
	param = document.createElement('PARAM');
	param.setAttribute('name', 'FlashVars');
	param.setAttribute('value', flashvars);
	base.appendChild(param);
	param = document.createElement('PARAM');
	param.setAttribute('name', 'allowFullScreen');
	param.setAttribute('value', 'true');
	base.appendChild(param);

	// Firefox support
	embed = document.createElement('EMBED');
	embed.setAttribute('src', src);
	embed.setAttribute('flashvars', flashvars);
	embed.setAttribute('allowfullscreen', 'true');
	embed.setAttribute('width', '640');
	embed.setAttribute('height', '480');
	base.appendChild(embed);

	document.body.appendChild(base);
	$('#flashgui').dialog({
		modal: true,
		width: 660,
		height: 480,
		title: elem + '\'s GUI',
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

function mo_refresh() {
	mo_pjs_reset();
	mo_properties('');
	mo_status();
}

function mo_slot_load(idx) {
	// do the real load
	$.get(mo_baseurl + '/pipeline/slot/load?idx=' + idx, function(data) {
		mo_refresh();
	});
}

function mo_slot_save(idx) {
	$.get(mo_baseurl + '/pipeline/slot/save?idx=' + idx, function(data) {
	});
}

function mo_slot_refresh(callback) {
	$.get(mo_baseurl + '/pipeline/slot/refresh', function(data) {
		// refresh data
		for ( slotidx in data['slots'] ) {
			var slot = data['slots'][slotidx];
			var slotinfo = 'Never used';
			var lastmod = parseInt(slot['lastmod']);
			if ( lastmod > 0 ) {
				var date = new Date(lastmod * 1000);
				slotinfo = date.toDateString() + ' ' +
						   date.getHours() + ':' +
						   date.getMinutes() + ':' +
						   date.getSeconds();
			}
			$('#slot-' + (parseInt(slotidx)+1) + ' .details').html(slotinfo);
		}
		if (callback)
			callback();
	});
}

function mo_layout() {
	widgetAutoLayout();
}

// once our document is loaded, load our software.
$(document).ready(function() {
	mo_bootstrap();
});
