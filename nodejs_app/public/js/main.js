'use strict';

// Angular app setup and config controller
var myApp = angular.module('myApp',['ui.bootstrap-slider', 'ui.bootstrap']);
myApp.controller('configController', ['$scope', function ($scope) {
      $scope.default_config = {
        logSpeed: 50, // ms
        headingSampleRate: 50, // ms
        frontMotorRamp: 3000, // ms
        rearMotorRamp: 3000, // ms
        tickDistance: 90, // mm
        speedOfSound: 33350, // cm/s
        rangeMin: 2, // cm
        rangeMax: 400, // cm
        rangeSampleRate: 100, // ms
        pressureSampleRate: 2000, // ms
        degreeCorrection: 90, // degrees
        steeringLeftMax: 900,
        steeringRightMax: 100
      };

      $scope.config = angular.copy($scope.default_config);
      $scope.settings = {
        motorSelect:2,
        frontMotorGoal:100,
        rearMotorGoal:100,
        steeringMotorGoal:100,
        steeringAmountLeft: 100,
        steeringAmountRight: 100
      };

      $scope.$watch("config", function(newValue, oldValue) {
        var keys = [];
        var i = -1;
        var changes = _.omit(newValue, function(v,k) { 
          i++;
          if(oldValue[k] === v) {
            return true;
          } else {
            keys.push(i);
            return false;
          }
        });

        $.each(changes, function(key, value) {

          var enum_key = keys.shift();
          console.log(key, enum_key, value);
         
          setConfig(enum_key, value); 
        });
        
      }, true);
  }]);

var chartDataLength = 200;
var configScope

angular.element(document).ready(function() {

  configScope = angular.element(document.querySelector('[ng-controller="configController"]')).scope();
  

  // Default location (specified by hash)
  if(location.hash == '#dashboard') {
    toggleDashboard();
  } else if(location.hash == '#remote') {
    toggleRemote();
  } else if(location.hash == '#sensors') {
    toggleSensors();
  } else {
    toggleConsole();
  }

  //$("#toggle-serial").bootstrapSwitch();

  //$("#motor-front-speed-slider").slider();
  //$("#motor-front-acc-slider").slider();
  //$("#motor-rear-speed-slider").slider();
  //$("#motor-rear-acc-slider").slider();
  /*
  $("#motor-front-speed-slider").slider({
    value: 100,
    max:100,
    min:0,
    create: function( event, ui ) { $('#motor-front-speed-value').val(100); },
    change: function( event, ui ) { $('#motor-front-speed-value').val(ui.value); },
    slide: function( event, ui ) { $('#motor-front-speed-value').val(ui.value); }
  });
  $("#motor-rear-speed-slider").slider({
    value: 100,
    max:100,
    min:0,
    create: function( event, ui ) { $('#motor-rear-speed-value').val(100); },
    change: function( event, ui ) { configScope.$apply(function () { configScope.config.rearMotorGoal = ui.value; }); },
    slide: function( event, ui ) { $('#motor-rear-speed-value').val(ui.value); }
  });
  $("#motor-front-acc-slider").slider({
    value: 2000,
    max:10000,
    min:0,
    step:100,
    create: function( event, ui ) { $('#motor-front-acc-value').val(2000); },
    change: function( event, ui ) { $('#motor-front-acc-value').val(ui.value); },
    slide: function( event, ui ) { $('#motor-front-acc-value').val(ui.value); }
  });
  $("#motor-rear-acc-slider").slider({
    value: 2000,
    max:10000,
    min:0,
    step:100,
    create: function( event, ui ) { $('#motor-rear-acc-value').val(2000); },
    change: function( event, ui ) { $('#motor-rear-acc-value').val(ui.value); },
    slide: function( event, ui ) { $('#motor-rear-acc-value').val(ui.value); }
  });  */


  // Play/Pause
  $('#socketio-control').click(function() {
    if(isPaused) {
      $(this).children('i').removeClass('fa-play');
      $(this).children('i').addClass('fa-pause');
    } else {
      $(this).children('i').removeClass('fa-pause');
      $(this).children('i').addClass('fa-play');
    }
    isPaused = !isPaused;
  });

  // Charts
  /*myChart = $('#gaugeChart').epoch({
    type: 'time.gauge',
    value: 500,
    domain: [0, 1000],
    format: function(v) { return v.toFixed(2) + ' cm/s'; }
  });*/

  // Compass Image
  compassCanvas = document.getElementById("compassCanvas");
  compassContext = compassCanvas.getContext("2d");
  compassImage = document.createElement("img");
  needleImage = document.createElement("img");
  compassImage.onload=function() {
      compassContext.drawImage(compassImage,compassCanvas.width/2-compassImage.width/2,compassCanvas.height/2-compassImage.width/2);
  }
  needleImage.onload=function() {
      compassContext.drawImage(needleImage,compassCanvas.width/2-needleImage.width/2,compassCanvas.height/2-needleImage.height/2);
  }
  compassImage.src="/images/compass2.png";
  needleImage.src="/images/compass_needle.png";  
  
  chartCurrent.chart = new CanvasJS.Chart("chartCurrentContainer",{
    data: [{
      type: "line",
      dataPoints: chartCurrent.dataFront
    },{
      type: "line",
      dataPoints: chartCurrent.dataRear
    }],
    backgroundColor: "transparent",
    axisX: {
      lineThickness: 1,
      tickThickness: 1
    },
    axisY: {
      tickThickness:1,
      lineThickness:1, 
      gridThickness:1
    }
  });
  chartFrequency.chart = new CanvasJS.Chart("chartFrequencyContainer",{
    data: [{
      type: "line",
      dataPoints: chartFrequency.data
    }],
    backgroundColor: "transparent",
    axisX: {
      lineThickness: 1,
      tickThickness: 1
    },
    axisY: {
      tickThickness:1,
      lineThickness:1, 
      gridThickness:1
    }
  });  
  chartSpeed.chart = new CanvasJS.Chart("chartSpeedContainer",{
    data: [{
      type: "line",
      dataPoints: chartSpeed.data
    }],
    backgroundColor: "transparent",
    axisX: {
      lineThickness: 1,
      tickThickness: 1
    },
    axisY: {
      tickThickness:1,
      lineThickness:1, 
      gridThickness:1
    }
  }); 

  // Fill charts w/ zeros
  
  for(var c = 0; c < chartDataLength; c++) {
    updateChart(chartSpeed, 0);
    updateChart(chartFrequency, 0);
    updateCurrentChart(chartCurrent, 0, 0);
  }
  chartSpeed.chart.render(); 
  chartFrequency.chart.render();  
  chartCurrent.chart.render();

  
  $('.canvasjs-chart-credit').hide();
});

function toggleDashboard() {
  $('#nav-console-link').parent().removeClass('active');
  $('#nav-remote-link').parent().removeClass('active');
  $('#nav-sensors-link').parent().removeClass('active');
  $('#nav-dashboard-link').parent().addClass('active');
  $('#console').hide();
  $('#remote').hide();
  $('#sensors').hide();
  $('#dashboard').fadeIn('slow');
  location.hash = '#dashboard';
}

function toggleRemote() {
  $('#nav-console-link').parent().removeClass('active');
  $('#nav-dashboard-link').parent().removeClass('active');
  $('#nav-sensors-link').parent().removeClass('active');
  $('#nav-remote-link').parent().addClass('active');
  $('#console').hide();
  $('#dashboard').hide();
  $('#sensors').hide();
  $('#remote').fadeIn('slow');
  location.hash = '#remote';
}

function toggleSensors() {
  $('#nav-console-link').parent().removeClass('active');
  $('#nav-dashboard-link').parent().removeClass('active');
  $('#nav-remote-link').parent().removeClass('active');
  $('#nav-sensors-link').parent().addClass('active');
  $('#console').hide();
  $('#dashboard').hide();
  $('#remote').hide();
  $('#sensors').fadeIn('slow');
  location.hash = '#sensors';
}

function toggleConsole() {
  $('#nav-dashboard-link').parent().removeClass('active');
  $('#nav-remote-link').parent().removeClass('active');
  $('#nav-sensors-link').parent().removeClass('active');
  $('#nav-console-link').parent().addClass('active');
  $('#dashboard').hide();
  $('#remote').hide();
  $('#sensors').hide();
  $('#console').fadeIn('slow');
  location.hash = '#console';
}