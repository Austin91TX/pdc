//  - - debug toggle switch animation - - 
$(document).ready(function() {

	$('.onoffbtn').on('click', function(){
	  if($(this).children().is(':checked')){
	    $(this).addClass('active');
	  }
	  else{
	    $(this).removeClass('active')
	  }
	});
});

//  - - debug show/hide debug table - - 
$(document).on("change", "#log", function(){
            if($("#log:checked").length > 0 ) {
			  $("#debug-table").fadeIn(1000);
			}
			else {
			  $("#debug-table").fadeOut(1000);
			}
		});

//  - - show info dialog - - 
$(document).on("click", "#info", function(){
			  $("#info-dialog").fadeIn(1000);
		});

//  - - hide info dialog - - 
$(document).on("click", "#close", function(){
			  $("#info-dialog").fadeOut(1000);
		});


// Adding the active state to nav items (locally and on the server)
/*
$(document).ready(function() {
	var pathname = location.pathname.split("/")[1];
	$('nav a[href="'+pathname+'"]').parent().addClass('active');
})
*/



/* Accordion */
/*
$(document).ready(function() {

	$('.accordionHeader').toggleClass('inactiveHeader');
	$('.accordionHeader').click(function () {

		if($(this).is('.inactiveHeader')) {
			$(this).toggleClass('activeHeader').toggleClass('inactiveHeader');
			$(this).next().slideToggle().toggleClass('openContent');

		}

		else {
			$(this).toggleClass('activeHeader').toggleClass('inactiveHeader');
			$(this).next().slideToggle().toggleClass('openContent');
		}
	});
	return false;
});


$(document).ready(function() {
  $( ".fmhs-fab" ).click(function() {
    $( this ).toggleClass( "active" );
    $( '.top' ).toggleClass( "top-active" );
    $( '.message' ).toggleClass( "hide" );

    if ($('div').hasClass("active")){
      $( this ).click(function() {
        $('.snackbar').addClass("show");
      });
    }

    else
    $( this ).click(function() {
      $('.snackbar').removeClass("show");
    });
  });
*/








