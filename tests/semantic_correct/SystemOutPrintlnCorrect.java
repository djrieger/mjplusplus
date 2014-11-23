/* Should produce no semantic errors. */
class A {
	public static void main(String[] args) {
	System.out.println(0);
	B b = new B();
	b.init();
	b.printRet();
	}
}
class B {

	public int ret;
	public void init(){
		System.out.println(1); 
		System System = new System();
		System.out = new Out();
		ret = System.out.println(System());	
	}
	public int System() {return 2;}
	public void printRet() {System.out.println(ret);}
}
class System {
	public Out out;
}

class Out {
	public int println(int a) {
		System.out.println(a);
		return 3;
	}
}