$(document).ready(function(){

    $(".button").click(function(){
        $(this).addClass("active").siblings().removeClass("active");

        var filter = $(this).attr("data-filter");
    
        if (filter == "all"){
            $(".gallery-item.image").show(400);
        }
        else{
            $(".gallery-item.image").not("."+filter).hide(200);
            $(".gallery-item.image").filter("."+filter).show(400);
        }
    })

    //MAGNIFIC-POPUP
    $(".gallery").magnificPopup({
        
        delegate: "a",
        type: "image",
        gallery:{
            enabled: true
        }
    })


});