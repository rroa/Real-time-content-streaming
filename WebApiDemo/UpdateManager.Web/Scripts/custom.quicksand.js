$(function () {
    $('.button').click(function (e) {
        $('.all').quicksand($('.warm li'), {
            duration: 3000,
            attribute: 'id'
        });
        e.preventDefault();
    });
});