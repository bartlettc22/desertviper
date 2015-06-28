'use strict';

//var app = ;

angular.module('myApp', ['ngResource'])

  .factory('householdChores', ['$resource', function($resource){
    return $resource('/householdChores/:id', null, {
      'update': { method:'PUT' }
    });
  }])
	.controller('MyController', ['$scope', 'householdChores', function ($scope, householdChores) {
    
    $scope.chores = householdChores.query();

    $scope.save = function(){
    	console.log($scope.newChore);
      if(!$scope.newChore || $scope.newChore.length < 1) return;

      var chore = new householdChores($scope.newChore);
      chore.$save(function(){
        $scope.chores.push(chore); // Push to db
        $scope.newChore = ''; // Reset form
        $('#myModal').modal('hide');
      });
		}

		$scope.remove = function(index){
      var chore = $scope.chores[index];
      householdChores.remove({id: chore._id}, function(){
        $scope.chores.splice(index, 1);
      });        
  	}

  	$scope.toggle_actions = function() {
      $(this).children('.collapse').collapse('toggle');
    };
	}]);

/*
var app = angular.module('homeApp', ['ng-sortable'])
    .controller('demo', ['$scope', function ($scope) {
        $scope.items = ['learn Sortable',
                      'use gn-sortable',
                     'Enjoy'];
    }]);
*/
var myChart;
var timestamp = ((new Date()).getTime() / 1000)|0;
var updateChart;
var count


$(function() {

  $("#toggle-serial").bootstrapSwitch();
  $("#motor-speed-slider").slider({
    value: 100,
    max:100,
    min:0,
    create: function( event, ui ) { $('#motor-speed-value').val(100); },
    change: function( event, ui ) { $('#motor-speed-value').val(ui.value); }
  });
  
 /*var dps = []; // dataPoints

    var chart = new CanvasJS.Chart("lineChart",{
      title :{
        text: "Live Random Data"
      },      
      data: [{
        type: "line",
        dataPoints: dps 
      }]
    });

    var xVal = 0;
    var yVal = 100; 
    var updateInterval = 20;
    var dataLength = 500; // number of dataPoints visible at any point

updateChart = function (count, value) {
  count = count || 1;
  // count is number of times loop runs to generate random dataPoints.
  
  for (var j = 0; j < count; j++) { 
    yVal = value;
    dps.push({
      x: xVal,
      y: yVal
    });
    xVal++;
  };
  if (dps.length > dataLength)
  {
    dps.shift();        
  }
  
  chart.render();   

};

    // generates first set of dataPoints
    updateChart(dataLength); */

    // update chart after specified time. 
    //setInterval(function(){updateChart()}, updateInterval); 

});

function toggleConsole() {
  $('#nav-dashboard-link').parent().removeClass('active');
  $('#nav-console-link').parent().addClass('active');
  $('#dashboard').hide();
  $('#console').fadeIn('slow');
}

function toggleDashboard() {
  $('#nav-console-link').parent().removeClass('active');
  $('#nav-dashboard-link').parent().addClass('active');
  $('#console').hide();
  $('#dashboard').fadeIn('slow');
}
