$fn=40;
union() {
	difference() {
		import("leftBracket.stl");
		union() {
			translate([39.5,-38.001,-5]) cube([14,3,6.1]);
			translate([39.5,-38.001,24]) cube([14,3,6.1]);
			color("red") translate([47.1,-33.1,26.7]) rotate([90,0,0]) cylinder(h=10,d=2,center=true);
			color("red") translate([47.1,-33.1,-1.5]) rotate([90,0,0]) cylinder(h=10,d=2,center=true);
		}
	}
	difference() {
		translate([23,-23,-8]) rotate([0,0,-30]) hull() {
			cylinder(h=2,d=10,center=true);
			translate([20,0,0]) cylinder(h=2,d=6,center=true);
		}
		translate([23,-23,-8]) rotate([0,0,-30]) hull() {
			cylinder(h=2,d=7.5,center=true);
			translate([19.5,0,0]) cylinder(h=2,d=4.5,center=true);
		}
	}
}
