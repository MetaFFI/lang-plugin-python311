
from typing import List

def f1(p1: float, p2: float, p3: int, p4: int, p5: int, p6: int, p7: int, p8: int, p9: int, p10: int, p11: bool, p12: str, p13: List[str], p14: List[int])-> List[str]:

	""" This function expects the parameters (in that order):
		double = 3.141592
	    float = 2.71f

	    int8 = -10
	    int16 = -20
	    int32 = -30
	    int64 = -40

	    uint8 = 50
	    uint16 = 60
	    uint32 = 70
	    uint64 = 80

	    bool = 1

	    string = "This is an input"
	    string[] = {"element one", "element two"}

	    bytes = {2, 4, 6, 8, 10}
	"""

	print("Hello from Python F1")

	if p1 != 3.141592:
		raise RuntimeError("p1 != 3.141592. p1 = "+str(p1))
	

	if p2 - 2.71 > 0.000001: # float in python are actually double, therefore using C-float (32bit) "looses" its accuracy as python-float (64bit)
		raise RuntimeError("p2 - 2.71 > 0.000001. p2 = "+str(p2))

	if p3 != -10:
		raise RuntimeError("p3 != -10. p3 = "+str(p3))
	

	if p4 != -20:
		raise RuntimeError("p4 != -20")
	

	if p5 != -30:
		raise RuntimeError("p5 != -30")
	

	if p6 != -40:
		raise RuntimeError("p6 != -40")
	

	if p7 != 50:
		raise RuntimeError("p7 != 50")
	

	if p8 != 60:
		raise RuntimeError("p8 != 60")
	

	if p9 != 70:
		raise RuntimeError("p9 != 70")
	

	if p10 != 80:
		raise RuntimeError("p10 != 80")
	

	if not p11:
		raise RuntimeError("p11 == false")
	

	if p12 != "This is an input":
		raise RuntimeError("p12 != \"This is an input\"")
	

	if len(p13) != 2:
		raise RuntimeError("len(p13) != 2")


	if p13[0] != "element one":
		raise RuntimeError("p13[0] != \"element one\"")


	if p13[1] != "element two":
		raise RuntimeError("p13[1] != \"element two\"")
	

	if len(p14) != 5:
		raise RuntimeError("len(p14) != 5")

	if p14[0] != 2 or p14[1] != 4 or p14[2] != 6 or p14[3] != 8 or p14[4] != 10:
		raise RuntimeError("p14[0] != 2 or p14[1] != 4 or p14[2] != 6 or p14[3] != 8 or p14[4] != 10")

	"""
		double = 0.57721
	    float = 3.359f
	
	    int8 = -11
	    int16 = -21
	    int32 = -31
	    int64 = -41
	
	    uint8 = 51
	    uint16 = 61
	    uint32 = 71
	    uint64 = 81
	
	    bool = 1
	
	    string = "This is an output"
	    string[] = ["return one", "return two"]
	
	    bytes = [20, 40, 60, 80, 100]
	"""	
	r1 = 0.57721
	r2 = 3.359
	
	r3 = -11
	r4 = -21
	r5 = -31
	r6 = -41

	r7 = 51
	r8 = 61
	r9 = 71
	r10 = 81

	r11 = True

	r12 = 'This is an output'
	r13 = ["return one", "return two"]
	
	r14 = [20, 40, 60, 80, 100]

	return r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14
