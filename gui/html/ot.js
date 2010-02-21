var ot_baseurl = 'http://127.0.0.1:7500';
var ot_available_inputs = [];
var ot_available_outputs = [];
var ot_streamscale = 2;

function ot_bootstrap() {
	ot_modules();
	ot_status();
}

function ot_modules() {
	$.get(ot_baseurl + '/factory/list', function(data) {
		$('#modules_content').html('');
		$(data['list']).each(function (index, elem) {
			$('#modules_content').append(
				$('<a></a>')
				.html(elem)
				.addClass('module')
				.attr('href', 'javascript:ot_create("' + elem + '")')
			);
		});
	});
}

function ot_status() {
	$.get(ot_baseurl + '/pipeline/status', function(data) {
		ot_available_inputs = [];
		ot_available_outputs = [];

		$('#status').html(data['status']['running'] == '0' ? 'stopped' : 'running');

		$('#instances_content').html('');
		for ( key in data['status']['modules'] ) {
			infos = data['status']['modules'][key];
			$('#instances_content').append(
				$('<a></a>')
				.html(key)
				.addClass('module')
				.attr('id', 'mod_' + key)
				.attr('href', 'javascript:ot_properties("' + key + '")')
			);

			for ( idx in infos['inputs'] ) {
				input = infos['inputs'][idx];
				ot_available_inputs[ot_available_inputs.length] = key + ':' + input['index'];
			}

			for ( idx in infos['outputs'] ) {
				output = infos['outputs'][idx];
				ot_available_outputs[ot_available_outputs.length] = key + ':' + output['index'];
			}
		}
	});
}

function ot_create(elem) {
	$.get(ot_baseurl + '/pipeline/create?objectname=' + elem, function(data) {
		ot_status();
		ot_properties(data['message']);
	});
}

function ot_remove(elem) {
	$.get(ot_baseurl + '/pipeline/remove?objectname=' + elem, function(data) {
		ot_status();
		ot_properties('');
	});
}

function ot_properties(elem) {
	if ( elem == '' ) {
		$('#commands_content').html('');
		$('#properties_content').html('');
		$('#inputs_content').html('');
		$('#outputs_content').html('');
		ot_status();
		return;
	}

	$.get(ot_baseurl + '/pipeline/status', function(data) {
		for ( key in data['status']['modules'] ) {
			if ( key != elem )
				continue;
			infos = data['status']['modules'][key];

			$('#commands_content').html('');
			$('#commands_content').append(
				$('<a></a>')
				.html('Delete')
				.attr('href', 'javascript:ot_remove("' + key + '")')
			);

			// update properties
			$('#properties_content').html('');
			for ( property in infos['properties'] ) {
				value = infos['properties'][property];
				$('#properties_content').append(
					$('<p></p>')
					.html(property)
				);
				$('#properties_content').append(
					$('<input></input>')
					.attr('type', 'text')
					.attr('value', value)
					.attr('onblur', 'javascript:ot_set(this, "' + elem + '", "' + property + '")')
				);
			}

			$('#inputs_content').html('');
			$('#outputs_content').html('');

			for ( idx in infos['inputs'] ) {
				input = infos['inputs'][idx];
				$('#inputs_content').append(
					$('<p></p>')
					.html(input['name'])
				);
			}

			for ( idx in infos['outputs'] ) {
				output = infos['outputs'][idx];

				$('#outputs_content').append(
					$('<a></a>')
					.html(output['name'])
					.attr('href', 'javascript:ot_stream("' + elem + '")')
				);
				for ( oidx in output['observers'] ) {
					observer = output['observers'][oidx];
					$('#outputs_content').append(
						$('<p></p>')
						.append(
							$('<a></a>')
							.html(observer)
							.attr('href', 'javascript:ot_properties("' + observer + '")')
						)
						.append(
							$('<a>remove</a>')
							.attr('href', 'javascript:ot_connect("' + observer + ':' + idx + '", "NULL", ' + idx + ', "' + elem + '")')
						)
					);
				}

				// prepare input list
				select = $('<select></select>')
					.attr('onchange', 'ot_connect(this.value, "' + key + '", ' + idx + ', "' + elem + '")');
				select.append($('<option></option>'));
				for ( iidx in ot_available_inputs ) {
					input = ot_available_inputs[iidx];
					select.append(
						$('<option></option>')
						.attr('value', input)
						.html(input)
					);
				}
				$('#outputs_content').append(select);
			}
		}
	});
}

function ot_set(input, id, key) {
	$.get(ot_baseurl + '/pipeline/set?objectname=' + id + '&name=' + key + '&value=' + input.value, function(data) {
		// TODO
	});
}

function ot_connect(input, output, outidx, baseobject) {
	list = input.split(':');
	input = list[0];
	inidx = list[1];

	$.get(ot_baseurl + '/pipeline/connect?in=' + input + '&out=' + output + '&inidx=' + inidx + '&outidx=' + outidx, function(data) {
		ot_properties(baseobject);
	});
}

function ot_start() {
	$.get(ot_baseurl + '/pipeline/start', function(data) {
		ot_status();
	});
}

function ot_stop() {
	ot_stream('');
	$.get(ot_baseurl + '/pipeline/stop', function(data) {
		ot_status();
	});
}

function ot_stream(elem) {
	if ( elem == '' ) {
		$('#streamid').html('-');
		$('#stream').attr('src', '/gui/nostream.png');
	} else {
		$('#streamid').html(elem);
		$('#stream').attr('src', ot_baseurl + '/pipeline/stream?objectname=' + elem + '&scale=' + ot_streamscale);
	}
}


$(document).ready(function() { ot_bootstrap(); });
