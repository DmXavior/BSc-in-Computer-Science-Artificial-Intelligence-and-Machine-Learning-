function addChar(input, character) {
	if(input.value == null || input.value == "0")
		input.value = character
	else
		input.value += character
}

function cos(form) {
	form.display.value = Math.cos(form.display.value);
}

function sin(form) {
	form.display.value = Math.sin(form.display.value);
}

function tan(form) {
	form.display.value = Math.tan(form.display.value);
}

function sqrt(form) {
	form.display.value = Math.sqrt(form.display.value);
}

function ln(form) {
	form.display.value = Math.log(form.display.value);
}

function exp(form) {
	form.display.value = Math.exp(form.display.value);
}

function deleteChar(input) {
	input.value = input.value.substring(0, input.value.length - 1)
}
var val = 0.0;
function percent(input) {
  val = input.value;
  input.value = input.value + "%";
}

function changeSign(input) {
	if(input.value.substring(0, 1) == "-")
		input.value = input.value.substring(1, input.value.length)
	else
		input.value = "-" + input.value
}

function compute(form) {
  //if (val !== 0.0) {
   // var percent = form.display.value;  
   // percent = pcent.substring(percent.indexOf("%")+1);
   // form.display.value = parseFloat(percent)/100 * val;
    //val = 0.0;
 // } else 
    form.display.value = eval(form.display.value);
  }


function square(form) {
	form.display.value = eval(form.display.value) * eval(form.display.value)
}

function checkNum(str) {
	for (var i = 0; i < str.length; i++) {
		var ch = str.charAt(i);
		if (ch < "0" || ch > "9") {
			if (ch != "/" && ch != "*" && ch != "+" && ch != "-" && ch != "."
				&& ch != "(" && ch!= ")" && ch != "%") {
				alert("invalid entry!")
				return false
				}
			}
		}
		return true
}

// UNIT TESTING //

// Assert function
function assert(condition, message) {
	if (!condition) {
	  throw new Error('Assertion failed: ' + message);
	}
  }
  
  // Trigonometric Functions
  function testTrigonometricFunctions() {
	// Test 1
	const form1 = { display: { value: 0 } };
	cos(form1);
	assert(form1.display.value === 1, 'Cosine function error');
  
	// Test 2
	const form2 = { display: { value: Math.PI / 2 } };
	sin(form2);
	assert(Math.abs(form2.display.value - 1) < 0.00001, 'Sine function error');
  
	// Test 3
	const form3 = { display: { value: Math.PI / 4 } };
	tan(form3);
	assert(Math.abs(form3.display.value - 1) < 0.00001, 'Tangent function error');
  }
  
  // Exponential and Logarithmic Functions
  function testExpAndLogFunctions() {
	// Test 1
	const form1 = { display: { value: 1 } };
	exp(form1);
	assert(Math.abs(form1.display.value - 2.71828) < 0.00001, 'Exp function error');
  
	// Test 2
	const form3 = { display: { value: 2 } };
	exp(form3);
	assert(Math.abs(form3.display.value - 7.38905) < 0.00001, 'Exp function error');
	
	// Test 3
	const form2 = { display: { value: Math.E } };
	ln(form2);
	assert(form2.display.value === 1, 'Log function error');
  
  }
  
  // Other Functions
  function testOtherFunctions() {
	// Test 1
	const input1 = { value: '1234' };
	deleteChar(input1);
	assert(input1.value === '123', 'Last char deletion error');
  
	// Test 2
	const input2 = { value: '-5' };
	changeSign(input2);
	assert(input2.value === '5', 'Sign change error');
  
	// Test 3
	const form3 = { display: { value: '3*3' } };
	compute(form3);
	assert(form3.display.value === 9, 'Computation error');
  }
  
  // Run the tests
  try {
	testTrigonometricFunctions();
	testExpAndLogFunctions();
	testOtherFunctions();
	console.log('All tests passed!');
  } catch (error) {
	console.error('Test failed:', error.message);
  }