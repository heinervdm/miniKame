$fn=40;
union() {
	import("leftLegB.stl");
	difference() {
		color("red") translate([47,-50,7]) rotate([90,38.5,0]) hull() {
			cylinder(h=2,d=9,center=true);
			translate([19.25,0,0]) cylinder(h=2,d=5.5,center=true);
		}
		translate([47,-50,7]) rotate([90,38.5,0]) hull() {
			cylinder(h=2,d=7.5,center=true);
			translate([19.5,0,0]) cylinder(h=2,d=4.5,center=true);
		}
	}
}
