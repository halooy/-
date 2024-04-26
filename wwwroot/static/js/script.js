$(document).ready(function(){

  $('.scroll-top').hide();

  $(window).on('scroll',function(){
    
    $('#menu-btn').removeClass('fa-times');
    $('.navbar').removeClass('active');

    // STICKY HEADER
    if($(window).scrollTop() > 0){
      $(".header").addClass("sticky");
    }else{
      $(".header").removeClass("sticky");
    }

  });

  /*--------------- Booking Table Toggler ---------------*/
  document.querySelector('.booking-btn').onclick = () =>{
    document.querySelector('.book-table').classList.toggle('active');
  }
  
  document.querySelector('#close-booking-form').onclick = () =>{
    document.querySelector('.book-table').classList.remove('active');
  }
  
  /*--------------- Navbar Toggler ---------------*/
  $('#menu-btn').click(function(){
    $(this).toggleClass('fa-times');
    $('.navbar').toggleClass('active');
  });

  /*--------------- Scroll-Top ---------------*/
  // On Load/Scroll
  $(window).on('load scroll',function(){

    $(window).scroll(function () {
      if ($(this).scrollTop() > 100) {
        $('.scroll-top').fadeIn();
      } else {
        $('.scroll-top').fadeOut();
      }
    });
  
  });

});


