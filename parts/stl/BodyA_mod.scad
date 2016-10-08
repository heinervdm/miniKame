$fn=6;
difference() {
	import("BodyBmod.stl");
	union() {
		color("red") translate([-25,0,16.5]) rotate([90,0,90]) cylinder(h=5,d=7.5,center=true);
		color("red") translate([25,0,16.5]) rotate([90,0,90]) cylinder(h=5,d=7.5,center=true);
	}
}
