// globals
var CLIENT_ID = '935467195884-lb90av41hknemud3rpqhqsrnjjao0kj8.apps.googleusercontent.com';
var PROJECT_KEY = "MmVARSzahTucVrqjtB0OI4GqESH0jih7g";
var SCOPES = ['https://www.googleapis.com/auth/spreadsheets'];

function checkAuth() {
  gapi.auth.authorize( {
    client_id: CLIENT_ID,
    scope: SCOPES.join(' '),
    immediate: true
  }, handleAuthResult);  
}

function handleAuthResult(authResult) {
  if (authResult && !authResult.error) {
    createDefaultProgramSelectors();
    callScriptFunction()
  } else {                      // create authorization buttons
    var programSelectors = document.getElementsByClassName('program-selector');
    programSelectors = Array.prototype.slice.call(programSelectors, 0);
    programSelectors.forEach(function(programSelector) {
      var authDiv = programSelector.appendChild(document.createElement('div'));
      authDiv.classList.add('program-auth-div');
      var authButton = authDiv.appendChild(document.createElement('button'));
      authButton.textContent = 'Authorize Access to Google Sheets';
      authButton.type = 'button';
      authButton.addEventListener('click', handleAuthClick);
    });    
    createDefaultProgramSelectors();
  }
}

function handleAuthClick(event) {
  gapi.auth.authorize({
    client_id: CLIENT_ID,
    scope: SCOPES,
    immediate: false
  }, handleAuthResult);
  return false;
}

function callScriptFunction() {
  var request = {function: 'getData'};
  var op = gapi.client.request({
    root: 'https://script.googleapis.com',
    path: 'v1/scripts/' + PROJECT_KEY + ':run',
    method: 'POST',
    body: request
  });
  op.execute(function(res) {
    var programList = res.response.result;
    createProgramSelectors(programList);
  });
}

function createDefaultProgramSelectors() {
  var programSelectors = document.getElementsByClassName("program-selector");
  programSelectors = Array.prototype.slice.call(programSelectors, 0);
  programSelectors.forEach(function(programSelector) {
    var selector = document.createElement('select');
    selector.name = 'program-selection';
    // add selection options
    var allOption = document.createElement('option');
    allOption.innerHTML = 'All'; allOption.value = 'All';
    var customOption = document.createElement('option');
    customOption.innerHTML = 'Custom'; customOption.value = 'Custom';
    selector.appendChild(allOption);
    selector.appendChild(customOption);
    programSelector.appendChild(selector); programSelector.appendChild(document.createElement('br'))
    // add textarea for customization
    var programTextArea = document.createElement('textarea');
    programTextArea.name = 'program-list'; programTextArea.rows = 10; programTextArea.cols = 50;
    programSelector.appendChild(programTextArea);
  });
}

function createProgramSelectors(programList) {
  var programMap = processProgramList(programList);
  var programSelectors = document.getElementsByClassName("program-selector");
  programSelectors = Array.prototype.slice.call(programSelectors, 0);
  programSelectors.forEach(function(programSelector) {
    // clean up auth divs if any
    var authDivs = programSelector.getElementsByClassName('program-auth-div');
    authDivs = Array.prototype.slice.call(authDivs, 0);
    authDivs.forEach(function(authDiv) { authDiv.parentNode.removeChild(authDiv); });
    // add google sheet info to selector
    var selector = programSelector.querySelector('select');
    Object.keys(programMap).forEach(function(key) {
      var option = document.createElement('option');
      option.value = key;
      option.innerHTML = key;
      selector.appendChild(option);
    }); 
    var programTextArea = programSelector.querySelector('textarea');
    selector.addEventListener("change", function(e) {
      var selectedValue = this.options[this.selectedIndex].value;
      if (selectedValue === "Custom") {
        programTextArea.textContent = '';
      } else if (selectedValue != 'All') {
        programTextArea.textContent = programMap[selectedValue].join('\n');
      }
    });
  });
}

function processProgramList(programList) {
  var programMap = {};
  if (programList.length == 0) return programMap;
  for (var i = 1; i < programList[0].length; ++i) {
    var listTitle = programList[0][i];
    programMap[listTitle] = [];
    for (var j = 1; j < programList.length; ++j) {
      if (programList[j][i] === 1) {
        programMap[listTitle].push(programList[j][0]);
      }
    }
  }
  return programMap;
}