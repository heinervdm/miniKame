$fn=50;
import("BodyAmod.stl");
translate([23,0,2.5]) difference() {
	cube([13,11,2], center=true);
	union() {
		translate([0,-3.4,0]) cylinder(h=10,d=2,center=true);
		translate([0,3.3,0]) cylinder(h=10,d=2,center=true);
	}
}
translate([23,32,2.5]) difference() {
	cube([13,6,2], center=true);
	translate([0,-0.7,0]) cylinder(h=10,d=2,center=true);
}
translate([23,-32,2.5]) difference() {
	cube([13,6,2], center=true);
	translate([0,0.8,0]) cylinder(h=10,d=2,center=true);
}
translate([-23,0,2.5]) difference() {
	cube([13,11,2], center=true);
	union() {
		translate([0,-3.4,0]) cylinder(h=10,d=2,center=true);
		translate([0,3.3,0]) cylinder(h=10,d=2,center=true);
	}
}
translate([-23,32,2.5]) difference() {
	cube([13,6,2], center=true);
	translate([0,-0.7,0]) cylinder(h=10,d=2,center=true);
}
translate([-23,-32,2.5]) difference() {
	cube([13,6,2], center=true);
	translate([0,0.8,0]) cylinder(h=10,d=2,center=true);
}
