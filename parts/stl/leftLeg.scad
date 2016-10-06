$fn=40;
//union() {
//	import("leftLegB.stl");
//	difference() {
//		color("red") translate([47,-50,7]) rotate([90,38.5,0]) hull() {
//			cylinder(h=2,d=9,center=true);
//			translate([19.25,0,0]) cylinder(h=2,d=5.5,center=true);
//		}
//		translate([47,-50,7]) rotate([90,38.5,0]) hull() {
//			cylinder(h=2,d=7.5,center=true);
//			translate([14,0,0]) cylinder(h=2,d=4.5,center=true);
//		}
//	}
//}
//translate([89,-27.5+1.5,-3]) {
mirror([0,1,0]) {
difference() {
	union() {
		difference() {
			union() {
				rotate([90,0,0]) cylinder(h=13,d=12,center=true);
				translate([-15/2,0,0]) cube([15,13,12],center=true);
			}
			rotate([90,0,0]) cylinder(h=15,d=3.5,center=true);
		}
		translate([0,37,0]) hull() {
			translate([-27,-22-0.5,0]) cube([5,3,12],center=true);
			translate([-42,-22-0.5,18-17/2]) rotate([90,0,0]) cylinder(h=3,d=17,center=true);
		}
		hull() {
			translate([-27,-22-1,0]) cube([5,4,12],center=true);
			translate([-42,-22-1,18-17/2]) rotate([90,0,0]) cylinder(h=4,d=17,center=true);
		}
		hull() {
			translate([-12.5,6.5,0]) cube([5,0.1,12],center=true);
			translate([-24.5,37-22-0.5,0]) cube([0.1,3,12],center=true);
		}
		hull() {
			translate([-13.5,-6.5,0]) cube([3,0.1,12],center=true);
			translate([-24.5,-22-1,0]) cube([0.1,4,12],center=true);
		}
	}
	union() {
		translate([-42,-25,18-17/2]) {
			rotate([90,0,0]) cylinder(h=10,d=8,center=true);
			rotate([90,38.5,0]) hull() {
				cylinder(h=4,d=8,center=true);
				translate([14,0,0]) cylinder(h=4,d=4.5,center=true);
			}
		}
		translate([-42,37-22-0.5,18-17/2]) {
			translate([0,-1,0]) rotate([90,0,0]) cylinder(h=6,d=8.5,center=true);
			translate([0,2,0]) rotate([90,0,0]) cylinder(h=2,d=10.5,center=true);
		}
	}
}
}