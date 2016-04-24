difference() {
	import("rightBracket.stl");
	union() {
		translate([-53.5,-38.001,-5]) cube([14,2,6.1]);
		translate([-53.5,-38.001,24]) cube([14,2,6.1]);
		$fn=20;
		color("red") translate([-47.1,-33.1,26.7]) rotate([90,0,0]) cylinder(h=10,d=2,center=true);
		color("red") translate([-47.1,-33.1,-1.5]) rotate([90,0,0]) cylinder(h=10,d=2,center=true);
	}
}