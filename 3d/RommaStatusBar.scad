bottom_width=16.5;
top_width=17;
height=115;
leds_per_meter=60; // [30, 60, 72, 94, 144]

function mm_between_leds(n) = 992.4/n;


//ws2812 with tolerances
module single_led() {
    w = 5.5;
    color("AliceBlue") translate([-w/2,-w/2,-0.1]) cube([w, w, 2.1]);
}

module led_strip(n, length, width=10.5) {
    thickness = 1;
    sep = mm_between_leds(n);
    for (x = [0 : sep : length]) {
        translate([(sep/2) + x, 0, thickness]) single_led();
    }
    min_length = ceil(length/sep)*sep;
    echo(min_length, sep);
    color("AliceBlue") translate([0,-width/2,0]) cube([min_length, width, thickness]);
}

//translate([0,0,10]) rotate([180,0,0]) led_strip(60, 10);

module basic_shape() {
    hull() {
        circle(d=bottom_width, $fn=50);
        translate([height-top_width,0,0]) square(top_width, center=true);
    };
};

romma_wall_thickness = 4;
outer_thickness = 3;
inner_extrusion = 5;
intersection() {
    difference() {

        union() {
        translate ([0,0,romma_wall_thickness]) linear_extrude(height = inner_extrusion, center = false, convexity = 10, twist = 0) scale([1.5, 1.5, 1])  basic_shape();
        color("Yellow") linear_extrude(height = romma_wall_thickness, center = false, convexity = 10, twist = 0) basic_shape();
        //translate ([0,0,-outer_thickness]) linear_extrude(height = outer_thickness, center = false, convexity = 10, twist = 0) scale([1.3, 1.3, 1])  basic_shape();
        }

    translate([-6.5,0,romma_wall_thickness+inner_extrusion+0.1]) rotate([180,0,0]) led_strip(leds_per_meter, height);
    }
    cube([height*2- top_width, height, height], center=true);
}
// top locking tooth
translate([height- top_width/2,0,romma_wall_thickness+3]) cube([5,12,6], center=true);

//translate([0, 30,0]) basic_shape();
