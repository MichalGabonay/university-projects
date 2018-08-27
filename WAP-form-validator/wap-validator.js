var inputFields = document.querySelectorAll('input[type="text"]');

var showError = function (field, error) {

    // Add error class to field
    field.classList.add('error');
    // Remove valid class to field
    field.classList.remove('valid');

    // Get field id or name
    var id = field.id || field.name;
    if (!id) return;

    // Check if error message field already exists
    // If not, create one
    var message = field.form.querySelector('.error-message#error-for-' + id );
    if (!message) {
        message = document.createElement('div');
        message.className = 'error-message';
        message.id = 'error-for-' + id;
        field.parentNode.insertBefore( message, field.nextSibling );
    }

    // Add ARIA role to the field
    field.setAttribute('aria-describedby', 'error-for-' + id);

    // Update error message
    message.innerHTML = error;

    // Show error message
    message.style.display = 'block';
    message.style.visibility = 'visible';

};

var notRequired = function(field) {
	// Remove error class to field
	field.classList.remove('error');
	// Remove valid class to field
    field.classList.remove('valid');

    // Get field id or name
    var id = field.id || field.name;
    if (!id) return;

    // Check if error message field exists
    var message = field.form.querySelector('.error-message#error-for-' + id );

    // Hide error message
    if (message) {
    	message.style.display = 'none';
    	message.style.visibility = 'none';
    }
};

var showValid = function (field) {

    // Remove error class to field
    field.classList.remove('error');
    // Add valid class to field
    field.classList.add('valid');

    // Get field id or name
    var id = field.id || field.name;
    if (!id) return;

    // Check if error message field exists
    var message = field.form.querySelector('.error-message#error-for-' + id );

    // Hide error message
    if (message) {
    	message.style.display = 'none';
    	message.style.visibility = 'none';
    }
};

var validate = function() {
	var error = "";
	var globalError = 0;
	var fieldError = 0;

	for (i=0; i<inputFields.length; i++){
		fieldError = 0;
		error = ""

		//check if field is not required
		if (inputFields[i].dataset.required != "true" && inputFields[i].value.length == 0) {
			fieldError = 0;
			notRequired(inputFields[i]);
		}else{
			//check for minimum requirement for field
			if (inputFields[i].dataset.min) {
				if (inputFields[i].value.length < inputFields[i].dataset.min) {
					error = "Minimálna dĺžka: " + inputFields[i].dataset.min;
					showError(inputFields[i], error);
					fieldError = 1;
					globalError = 1;
				}
			}

			//check for maximum requirement for field
			if (inputFields[i].dataset.max) {
				if (inputFields[i].value.length > inputFields[i].dataset.max) {
					error = "Maximálna dĺžka: " + inputFields[i].dataset.max;
					showError(inputFields[i], error);
					fieldError = 1;
					globalError = 1;
				}
				
			}

			//check for pattern requirement for field
			if (inputFields[i].dataset.pattern) {
				var pattern = new RegExp(inputFields[i].dataset.pattern);
				if (!pattern.test(inputFields[i].value)) {
					error = "Zadaný vstup má chybný formát";
					showError(inputFields[i], error);
					fieldError = 1;
					globalError = 1;
				}
			}

			//check if field is required
			if (inputFields[i].dataset.required) {
				if (inputFields[i].dataset.required == "true" && inputFields[i].value.length == 0) {
					error = "Toto pole je povinné.";
					showError(inputFields[i], error);
					fieldError = 1;
					globalError = 1;
				}
			}

			if (fieldError == 1) {
				showError(inputFields[i], error);
			}else{
				showValid(inputFields[i]);
			}
		}
	}
	//check for not good filled field in form, if isnt enable submit button
	if (globalError == 1) {
		document.querySelector('input[type="submit"]').disabled = true;
	}else{
		document.querySelector('input[type="submit"]').disabled = false;
	}
}

validate();

//check validatin for every typing in field
for (i=0; i<inputFields.length; i++){
	inputFields[i].oninput=function(){
		validate();
	}
}