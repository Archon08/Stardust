#!/usr/bin/env python3
# Death-spiral structural analysis from REAL extracted JTL data:
# per-chassis defensive turret coverage (can it bring weapons to bear on a fast crossing target?).
import sys, math, json, os
sys.path.insert(0, "/mnt/c/swg-extract")
from iff_datatable import parse_datatable

D = os.path.expanduser("~/swg-space-dt/datatables/space")
turret = parse_datatable(f"{D}/ship_turret.iff")["rows"]
chassis = parse_datatable(f"{D}/ship_chassis.iff")["rows"]
weap = {r["name"]: r for r in parse_datatable(f"{D}/ship_weapon_components.iff")["rows"]}

# fraction of the unit sphere covered by a yaw[min,max] x pitch[min,max] window
def coverage(minYaw, maxYaw, minPitch, maxPitch):
    yspan = math.radians(min(360.0, maxYaw - minYaw))
    # solid angle of a lat/long window = yspan * (sin(maxPitch) - sin(minPitch))
    sa = yspan * (math.sin(math.radians(maxPitch)) - math.sin(math.radians(minPitch)))
    return max(0.0, sa) / (4 * math.pi)

# aggregate turret coverage per chassis
by_chassis = {}
for t in turret:
    c = t["chassis"]
    by_chassis.setdefault(c, []).append(coverage(t["minYaw"], t["maxYaw"], t["minPitch"], t["maxPitch"]))

# classify every chassis that has weapon slots
defenseless = []   # weapons but ~no turret arc (fixed-forward only): farmable by interceptors
protected = []     # full-sphere turret coverage: interceptor-proof
for ch in chassis:
    name = ch["name"]
    nweap = sum(1 for i in range(8) if ch.get(f"weapon_{i}"))
    if nweap == 0:
        continue
    arcs = by_chassis.get(name, [])
    best = max(arcs) if arcs else 0.0          # best single-weapon coverage
    total = min(1.0, sum(arcs))                # combined (overlapping ignored -> capped)
    rec = {"chassis": name, "weapons": nweap, "turret_slots": len(arcs),
           "best_cov": round(best, 3), "total_cov": round(total, 3)}
    if total < 0.06:        # < ~6% of sphere reachable: effectively fixed-forward
        defenseless.append(rec)
    elif total > 0.5:
        protected.append(rec)

allwslots = sum(1 for ch in chassis for i in range(8) if ch.get(f"weapon_{i}"))
nflight = len(defenseless); nprot = len(protected)
ntot = sum(1 for ch in chassis if any(ch.get(f"weapon_{i}") for i in range(8)))

print("=== JTL death-spiral structural analysis (REAL extracted data) ===")
print(f"armed chassis: {ntot}")
print(f"  effectively FIXED-FORWARD (total turret coverage <6% of sphere): {nflight}")
print(f"  TURRET-PROTECTED (>50% coverage): {nprot}")
print(f"  ratio fixed:protected = {nflight}:{nprot}")
print()
print("--- representative FIXED-FORWARD (farmable by faster attackers) ---")
for r in sorted(defenseless, key=lambda x:x['total_cov'])[:18]:
    print(f"  {r['chassis']:<34} weapons={r['weapons']} turretArcs={r['turret_slots']} totalCov={r['total_cov']}")
print()
print("--- representative TURRET-PROTECTED ---")
for r in sorted(protected, key=lambda x:-x['total_cov'])[:10]:
    print(f"  {r['chassis']:<34} weapons={r['weapons']} turretArcs={r['turret_slots']} totalCov={r['total_cov']}")
print()
# weapon range spread (engagement-distance half of the spiral)
ranges = sorted(set(round(w["range"]) for w in weap.values() if w["range"] > 0))
print(f"weapon ranges present: {ranges[:12]}{' ...' if len(ranges)>12 else ''}")
