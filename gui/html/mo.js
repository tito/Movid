var mo_baseurl = 'http://127.0.0.1:7500';
var mo_available_inputs = [];
var mo_available_outputs = [];
var mo_streamscale = 2;

function mo_bootstrap() {
	mo_modules();
	mo_status();
}

function mo_modules() {
	$.get(mo_baseurl + '/factory/list', function(data) {
		$('#modules_content').html('');
		$(data['list']).each(function (index, elem) {
			$('#modules_content').append(
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

		$('#status').html(data['status']['running'] == '0' ? 'stopped' : 'running');

		$('#instances_content').html('');
		for ( key in data['status']['modules'] ) {
			infos = data['status']['modules'][key];
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
		}
	});
}

function mo_create(elem) {
	$.get(mo_baseurl + '/pipeline/create?objectname=' + elem, function(data) {
		mo_status();
		mo_properties(data['message']);
	});
}

function mo_remove(elem) {
	$.get(mo_baseurl + '/pipeline/remove?objectname=' + elem, function(data) {
		mo_status();
		mo_properties('');
	});
}

function mo_properties(elem) {
	if ( elem == '' ) {
		$('#commands_content').html('');
		$('#properties_content').html('');
		$('#inputs_content').html('');
		$('#outputs_content').html('');
		mo_status();
		return;
	}

	$.get(mo_baseurl + '/pipeline/status', function(data) {
		for ( key in data['status']['modules'] ) {
			if ( key != elem )
				continue;
			infos = data['status']['modules'][key];

			$('#commands_content').html('');
			$('#commands_content').append(
				$('<a></a>')
				.html('Delete')
				.attr('href', 'javascript:mo_remove("' + key + '")')
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
					.attr('onblur', 'javascript:mo_set(this, "' + elem + '", "' + property + '")')
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
					.attr('href', 'javascript:mo_stream("' + elem + '")')
				);
				for ( oidx in output['observers'] ) {
					observer = output['observers'][oidx];
					$('#outputs_content').append(
						$('<p></p>')
						.append(
							$('<a></a>')
							.html(observer)
							.attr('href', 'javascript:mo_properties("' + observer + '")')
						)
						.append(
							$('<a>remove</a>')
							.attr('href', 'javascript:mo_connect("' + observer + ':' + idx + '", "NULL", ' + idx + ', "' + elem + '")')
						)
					);
				}

				// prepare input list
				select = $('<select></select>')
					.attr('onchange', 'mo_connect(this.value, "' + key + '", ' + idx + ', "' + elem + '")');
				select.append($('<option></option>'));
				for ( iidx in mo_available_inputs ) {
					input = mo_available_inputs[iidx];
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

function mo_set(input, id, key) {
	$.get(mo_baseurl + '/pipeline/set?objectname=' + id + '&name=' + key + '&value=' + input.value, function(data) {
		// TODO
	});
}

function mo_connect(input, output, outidx, baseobject) {
	list = input.split(':');
	input = list[0];
	inidx = list[1];

	$.get(mo_baseurl + '/pipeline/connect?in=' + input + '&out=' + output + '&inidx=' + inidx + '&outidx=' + outidx, function(data) {
		mo_properties(baseobject);
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
	if ( elem == '' ) {
		$('#streamid').html('-');
		$('#stream').attr('src', '/gui/nostream.png');
	} else {
		$('#streamid').html(elem);
		$('#stream').attr('src', mo_baseurl + '/pipeline/stream?objectname=' + elem + '&scale=' + mo_streamscale);
	}
}


$(document).ready(function() { mo_bootstrap(); });
