/*--------------- Home Slider ---------------*/ 
var swiper = new Swiper(".home-slider", {

    loop:true,
    autoplay: {
      delay: 5000,
      disableOnInteraction: false, 
    },

    pagination: {
      el: ".swiper-pagination2",
      clickable:true,
    },

}); 