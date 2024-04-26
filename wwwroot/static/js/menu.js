/*---------- Menu Gallery ----------*/

var productTabs = document.querySelector('.menu .menu-btns');

productTabs.addEventListener('click', function(e){

    if(e.target.classList.contains('menu-btn') && !e.target.classList.contains('active')){
        
        var target = e.target.getAttribute('data-target');
        productTabs.querySelector('.active').classList.remove('active');
        e.target.classList.add('active');

        var productSection = document.querySelector('.menu');
        productSection.querySelector('.menu-gallery.active').classList.remove('active');
        productSection.querySelector(target).classList.add('active');

    }
})