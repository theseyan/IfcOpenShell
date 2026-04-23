// THIS FILE IS AUTO-GENERATED. DO NOT EDIT.
// Source: IFC2X3_TC1.exp
// Schema: IFC2X3

#include "ifcapi/express/runtime.h"
#include <cmath>

namespace ifcapi { namespace express { namespace IFC2X3 {

using ifcapi::express::Value;
using ifcapi::express::EntityRef;

// --- EXPRESS literals ---
inline const Value unknown = Value();  // EXPRESS unknown logical
inline const Value indeterminate = Value();

// --- enum constants ---
inline const Value dead_load_g = Value(std::string("DEAD_LOAD_G"));
inline const Value completion_g1 = Value(std::string("COMPLETION_G1"));
inline const Value live_load_q = Value(std::string("LIVE_LOAD_Q"));
inline const Value snow_s = Value(std::string("SNOW_S"));
inline const Value wind_w = Value(std::string("WIND_W"));
inline const Value prestressing_p = Value(std::string("PRESTRESSING_P"));
inline const Value settlement_u = Value(std::string("SETTLEMENT_U"));
inline const Value temperature_t = Value(std::string("TEMPERATURE_T"));
inline const Value earthquake_e = Value(std::string("EARTHQUAKE_E"));
inline const Value fire = Value(std::string("FIRE"));
inline const Value impulse = Value(std::string("IMPULSE"));
inline const Value impact = Value(std::string("IMPACT"));
inline const Value transport = Value(std::string("TRANSPORT"));
inline const Value erection = Value(std::string("ERECTION"));
inline const Value propping = Value(std::string("PROPPING"));
inline const Value system_imperfection = Value(std::string("SYSTEM_IMPERFECTION"));
inline const Value shrinkage = Value(std::string("SHRINKAGE"));
inline const Value creep = Value(std::string("CREEP"));
inline const Value lack_of_fit = Value(std::string("LACK_OF_FIT"));
inline const Value buoyancy = Value(std::string("BUOYANCY"));
inline const Value ice = Value(std::string("ICE"));
inline const Value current = Value(std::string("CURRENT"));
inline const Value wave = Value(std::string("WAVE"));
inline const Value rain = Value(std::string("RAIN"));
inline const Value brakes = Value(std::string("BRAKES"));
inline const Value userdefined = Value(std::string("USERDEFINED"));
inline const Value notdefined = Value(std::string("NOTDEFINED"));
inline const Value permanent_g = Value(std::string("PERMANENT_G"));
inline const Value variable_q = Value(std::string("VARIABLE_Q"));
inline const Value extraordinary_a = Value(std::string("EXTRAORDINARY_A"));
inline const Value electricactuator = Value(std::string("ELECTRICACTUATOR"));
inline const Value handoperatedactuator = Value(std::string("HANDOPERATEDACTUATOR"));
inline const Value hydraulicactuator = Value(std::string("HYDRAULICACTUATOR"));
inline const Value pneumaticactuator = Value(std::string("PNEUMATICACTUATOR"));
inline const Value thermostaticactuator = Value(std::string("THERMOSTATICACTUATOR"));
inline const Value office = Value(std::string("OFFICE"));
inline const Value site = Value(std::string("SITE"));
inline const Value home = Value(std::string("HOME"));
inline const Value distributionpoint = Value(std::string("DISTRIBUTIONPOINT"));
inline const Value ahead = Value(std::string("AHEAD"));
inline const Value behind = Value(std::string("BEHIND"));
inline const Value constantflow = Value(std::string("CONSTANTFLOW"));
inline const Value variableflowpressuredependant = Value(std::string("VARIABLEFLOWPRESSUREDEPENDANT"));
inline const Value variableflowpressureindependant = Value(std::string("VARIABLEFLOWPRESSUREINDEPENDANT"));
inline const Value grille = Value(std::string("GRILLE"));
inline const Value register_ = Value(std::string("REGISTER"));
inline const Value diffuser = Value(std::string("DIFFUSER"));
inline const Value eyeball = Value(std::string("EYEBALL"));
inline const Value iris = Value(std::string("IRIS"));
inline const Value lineargrille = Value(std::string("LINEARGRILLE"));
inline const Value lineardiffuser = Value(std::string("LINEARDIFFUSER"));
inline const Value fixedplatecounterflowexchanger = Value(std::string("FIXEDPLATECOUNTERFLOWEXCHANGER"));
inline const Value fixedplatecrossflowexchanger = Value(std::string("FIXEDPLATECROSSFLOWEXCHANGER"));
inline const Value fixedplateparallelflowexchanger = Value(std::string("FIXEDPLATEPARALLELFLOWEXCHANGER"));
inline const Value rotarywheel = Value(std::string("ROTARYWHEEL"));
inline const Value runaroundcoilloop = Value(std::string("RUNAROUNDCOILLOOP"));
inline const Value heatpipe = Value(std::string("HEATPIPE"));
inline const Value twintowerenthalpyrecoveryloops = Value(std::string("TWINTOWERENTHALPYRECOVERYLOOPS"));
inline const Value thermosiphonsealedtubeheatexchangers = Value(std::string("THERMOSIPHONSEALEDTUBEHEATEXCHANGERS"));
inline const Value thermosiphoncoiltypeheatexchangers = Value(std::string("THERMOSIPHONCOILTYPEHEATEXCHANGERS"));
inline const Value bell = Value(std::string("BELL"));
inline const Value breakglassbutton = Value(std::string("BREAKGLASSBUTTON"));
inline const Value light = Value(std::string("LIGHT"));
inline const Value manualpullbox = Value(std::string("MANUALPULLBOX"));
inline const Value siren = Value(std::string("SIREN"));
inline const Value whistle = Value(std::string("WHISTLE"));
inline const Value in_plane_loading_2d = Value(std::string("IN_PLANE_LOADING_2D"));
inline const Value out_plane_loading_2d = Value(std::string("OUT_PLANE_LOADING_2D"));
inline const Value loading_3d = Value(std::string("LOADING_3D"));
inline const Value first_order_theory = Value(std::string("FIRST_ORDER_THEORY"));
inline const Value second_order_theory = Value(std::string("SECOND_ORDER_THEORY"));
inline const Value third_order_theory = Value(std::string("THIRD_ORDER_THEORY"));
inline const Value full_nonlinear_theory = Value(std::string("FULL_NONLINEAR_THEORY"));
inline const Value add = Value(std::string("ADD"));
inline const Value divide = Value(std::string("DIVIDE"));
inline const Value multiply = Value(std::string("MULTIPLY"));
inline const Value subtract = Value(std::string("SUBTRACT"));
inline const Value factory = Value(std::string("FACTORY"));
inline const Value polyline_form = Value(std::string("POLYLINE_FORM"));
inline const Value circular_arc = Value(std::string("CIRCULAR_ARC"));
inline const Value elliptic_arc = Value(std::string("ELLIPTIC_ARC"));
inline const Value parabolic_arc = Value(std::string("PARABOLIC_ARC"));
inline const Value hyperbolic_arc = Value(std::string("HYPERBOLIC_ARC"));
inline const Value unspecified = Value(std::string("UNSPECIFIED"));
inline const Value beam = Value(std::string("BEAM"));
inline const Value joist = Value(std::string("JOIST"));
inline const Value lintel = Value(std::string("LINTEL"));
inline const Value t_beam = Value(std::string("T_BEAM"));
inline const Value greaterthan = Value(std::string("GREATERTHAN"));
inline const Value greaterthanorequalto = Value(std::string("GREATERTHANOREQUALTO"));
inline const Value lessthan = Value(std::string("LESSTHAN"));
inline const Value lessthanorequalto = Value(std::string("LESSTHANOREQUALTO"));
inline const Value equalto = Value(std::string("EQUALTO"));
inline const Value notequalto = Value(std::string("NOTEQUALTO"));
inline const Value water = Value(std::string("WATER"));
inline const Value steam = Value(std::string("STEAM"));
inline const Value union_ = Value(std::string("UNION"));
inline const Value intersection = Value(std::string("INTERSECTION"));
inline const Value difference = Value(std::string("DIFFERENCE"));
inline const Value bend = Value(std::string("BEND"));
inline const Value cross = Value(std::string("CROSS"));
inline const Value reducer = Value(std::string("REDUCER"));
inline const Value tee = Value(std::string("TEE"));
inline const Value cableladdersegment = Value(std::string("CABLELADDERSEGMENT"));
inline const Value cabletraysegment = Value(std::string("CABLETRAYSEGMENT"));
inline const Value cabletrunkingsegment = Value(std::string("CABLETRUNKINGSEGMENT"));
inline const Value conduitsegment = Value(std::string("CONDUITSEGMENT"));
inline const Value cablesegment = Value(std::string("CABLESEGMENT"));
inline const Value conductorsegment = Value(std::string("CONDUCTORSEGMENT"));
inline const Value nochange = Value(std::string("NOCHANGE"));
inline const Value modified = Value(std::string("MODIFIED"));
inline const Value added = Value(std::string("ADDED"));
inline const Value deleted = Value(std::string("DELETED"));
inline const Value modifiedadded = Value(std::string("MODIFIEDADDED"));
inline const Value modifieddeleted = Value(std::string("MODIFIEDDELETED"));
inline const Value aircooled = Value(std::string("AIRCOOLED"));
inline const Value watercooled = Value(std::string("WATERCOOLED"));
inline const Value heatrecovery = Value(std::string("HEATRECOVERY"));
inline const Value dxcoolingcoil = Value(std::string("DXCOOLINGCOIL"));
inline const Value watercoolingcoil = Value(std::string("WATERCOOLINGCOIL"));
inline const Value steamheatingcoil = Value(std::string("STEAMHEATINGCOIL"));
inline const Value waterheatingcoil = Value(std::string("WATERHEATINGCOIL"));
inline const Value electricheatingcoil = Value(std::string("ELECTRICHEATINGCOIL"));
inline const Value gasheatingcoil = Value(std::string("GASHEATINGCOIL"));
inline const Value column = Value(std::string("COLUMN"));
inline const Value dynamic = Value(std::string("DYNAMIC"));
inline const Value reciprocating = Value(std::string("RECIPROCATING"));
inline const Value rotary = Value(std::string("ROTARY"));
inline const Value scroll = Value(std::string("SCROLL"));
inline const Value trochoidal = Value(std::string("TROCHOIDAL"));
inline const Value singlestage = Value(std::string("SINGLESTAGE"));
inline const Value booster = Value(std::string("BOOSTER"));
inline const Value opentype = Value(std::string("OPENTYPE"));
inline const Value hermetic = Value(std::string("HERMETIC"));
inline const Value semihermetic = Value(std::string("SEMIHERMETIC"));
inline const Value weldedshellhermetic = Value(std::string("WELDEDSHELLHERMETIC"));
inline const Value rollingpiston = Value(std::string("ROLLINGPISTON"));
inline const Value rotaryvane = Value(std::string("ROTARYVANE"));
inline const Value singlescrew = Value(std::string("SINGLESCREW"));
inline const Value twinscrew = Value(std::string("TWINSCREW"));
inline const Value watercooledshelltube = Value(std::string("WATERCOOLEDSHELLTUBE"));
inline const Value watercooledshellcoil = Value(std::string("WATERCOOLEDSHELLCOIL"));
inline const Value watercooledtubeintube = Value(std::string("WATERCOOLEDTUBEINTUBE"));
inline const Value watercooledbrazedplate = Value(std::string("WATERCOOLEDBRAZEDPLATE"));
inline const Value evaporativecooled = Value(std::string("EVAPORATIVECOOLED"));
inline const Value atpath = Value(std::string("ATPATH"));
inline const Value atstart = Value(std::string("ATSTART"));
inline const Value atend = Value(std::string("ATEND"));
inline const Value hard = Value(std::string("HARD"));
inline const Value soft = Value(std::string("SOFT"));
inline const Value advisory = Value(std::string("ADVISORY"));
inline const Value floating = Value(std::string("FLOATING"));
inline const Value proportional = Value(std::string("PROPORTIONAL"));
inline const Value proportionalintegral = Value(std::string("PROPORTIONALINTEGRAL"));
inline const Value proportionalintegralderivative = Value(std::string("PROPORTIONALINTEGRALDERIVATIVE"));
inline const Value timedtwoposition = Value(std::string("TIMEDTWOPOSITION"));
inline const Value twoposition = Value(std::string("TWOPOSITION"));
inline const Value active = Value(std::string("ACTIVE"));
inline const Value passive = Value(std::string("PASSIVE"));
inline const Value naturaldraft = Value(std::string("NATURALDRAFT"));
inline const Value mechanicalinduceddraft = Value(std::string("MECHANICALINDUCEDDRAFT"));
inline const Value mechanicalforceddraft = Value(std::string("MECHANICALFORCEDDRAFT"));
inline const Value budget = Value(std::string("BUDGET"));
inline const Value costplan = Value(std::string("COSTPLAN"));
inline const Value estimate = Value(std::string("ESTIMATE"));
inline const Value tender = Value(std::string("TENDER"));
inline const Value pricedbillofquantities = Value(std::string("PRICEDBILLOFQUANTITIES"));
inline const Value unpricedbillofquantities = Value(std::string("UNPRICEDBILLOFQUANTITIES"));
inline const Value scheduleofrates = Value(std::string("SCHEDULEOFRATES"));
inline const Value ceiling = Value(std::string("CEILING"));
inline const Value flooring = Value(std::string("FLOORING"));
inline const Value cladding = Value(std::string("CLADDING"));
inline const Value roofing = Value(std::string("ROOFING"));
inline const Value insulation = Value(std::string("INSULATION"));
inline const Value membrane = Value(std::string("MEMBRANE"));
inline const Value sleeving = Value(std::string("SLEEVING"));
inline const Value wrapping = Value(std::string("WRAPPING"));
inline const Value aed = Value(std::string("AED"));
inline const Value aes = Value(std::string("AES"));
inline const Value ats = Value(std::string("ATS"));
inline const Value aud = Value(std::string("AUD"));
inline const Value bbd = Value(std::string("BBD"));
inline const Value beg = Value(std::string("BEG"));
inline const Value bgl = Value(std::string("BGL"));
inline const Value bhd = Value(std::string("BHD"));
inline const Value bmd = Value(std::string("BMD"));
inline const Value bnd = Value(std::string("BND"));
inline const Value brl = Value(std::string("BRL"));
inline const Value bsd = Value(std::string("BSD"));
inline const Value bwp = Value(std::string("BWP"));
inline const Value bzd = Value(std::string("BZD"));
inline const Value cad = Value(std::string("CAD"));
inline const Value cbd = Value(std::string("CBD"));
inline const Value chf = Value(std::string("CHF"));
inline const Value clp = Value(std::string("CLP"));
inline const Value cny = Value(std::string("CNY"));
inline const Value cys = Value(std::string("CYS"));
inline const Value czk = Value(std::string("CZK"));
inline const Value ddp = Value(std::string("DDP"));
inline const Value dem = Value(std::string("DEM"));
inline const Value dkk = Value(std::string("DKK"));
inline const Value egl = Value(std::string("EGL"));
inline const Value est = Value(std::string("EST"));
inline const Value eur = Value(std::string("EUR"));
inline const Value fak = Value(std::string("FAK"));
inline const Value fim = Value(std::string("FIM"));
inline const Value fjd = Value(std::string("FJD"));
inline const Value fkp = Value(std::string("FKP"));
inline const Value frf = Value(std::string("FRF"));
inline const Value gbp = Value(std::string("GBP"));
inline const Value gip = Value(std::string("GIP"));
inline const Value gmd = Value(std::string("GMD"));
inline const Value grx = Value(std::string("GRX"));
inline const Value hkd = Value(std::string("HKD"));
inline const Value huf = Value(std::string("HUF"));
inline const Value ick = Value(std::string("ICK"));
inline const Value idr = Value(std::string("IDR"));
inline const Value ils = Value(std::string("ILS"));
inline const Value inr = Value(std::string("INR"));
inline const Value irp = Value(std::string("IRP"));
inline const Value itl = Value(std::string("ITL"));
inline const Value jmd = Value(std::string("JMD"));
inline const Value jod = Value(std::string("JOD"));
inline const Value jpy = Value(std::string("JPY"));
inline const Value kes = Value(std::string("KES"));
inline const Value krw = Value(std::string("KRW"));
inline const Value kwd = Value(std::string("KWD"));
inline const Value kyd = Value(std::string("KYD"));
inline const Value lkr = Value(std::string("LKR"));
inline const Value luf = Value(std::string("LUF"));
inline const Value mtl = Value(std::string("MTL"));
inline const Value mur = Value(std::string("MUR"));
inline const Value mxn = Value(std::string("MXN"));
inline const Value myr = Value(std::string("MYR"));
inline const Value nlg = Value(std::string("NLG"));
inline const Value nzd = Value(std::string("NZD"));
inline const Value omr = Value(std::string("OMR"));
inline const Value pgk = Value(std::string("PGK"));
inline const Value php = Value(std::string("PHP"));
inline const Value pkr = Value(std::string("PKR"));
inline const Value pln = Value(std::string("PLN"));
inline const Value ptn = Value(std::string("PTN"));
inline const Value qar = Value(std::string("QAR"));
inline const Value rur = Value(std::string("RUR"));
inline const Value sar = Value(std::string("SAR"));
inline const Value scr = Value(std::string("SCR"));
inline const Value sek = Value(std::string("SEK"));
inline const Value sgd = Value(std::string("SGD"));
inline const Value skp = Value(std::string("SKP"));
inline const Value thb = Value(std::string("THB"));
inline const Value trl = Value(std::string("TRL"));
inline const Value ttd = Value(std::string("TTD"));
inline const Value twd = Value(std::string("TWD"));
inline const Value usd = Value(std::string("USD"));
inline const Value veb = Value(std::string("VEB"));
inline const Value vnd = Value(std::string("VND"));
inline const Value xeu = Value(std::string("XEU"));
inline const Value zar = Value(std::string("ZAR"));
inline const Value zwd = Value(std::string("ZWD"));
inline const Value nok = Value(std::string("NOK"));
inline const Value controldamper = Value(std::string("CONTROLDAMPER"));
inline const Value firedamper = Value(std::string("FIREDAMPER"));
inline const Value smokedamper = Value(std::string("SMOKEDAMPER"));
inline const Value firesmokedamper = Value(std::string("FIRESMOKEDAMPER"));
inline const Value backdraftdamper = Value(std::string("BACKDRAFTDAMPER"));
inline const Value reliefdamper = Value(std::string("RELIEFDAMPER"));
inline const Value blastdamper = Value(std::string("BLASTDAMPER"));
inline const Value gravitydamper = Value(std::string("GRAVITYDAMPER"));
inline const Value gravityreliefdamper = Value(std::string("GRAVITYRELIEFDAMPER"));
inline const Value balancingdamper = Value(std::string("BALANCINGDAMPER"));
inline const Value fumehoodexhaust = Value(std::string("FUMEHOODEXHAUST"));
inline const Value measured = Value(std::string("MEASURED"));
inline const Value predicted = Value(std::string("PREDICTED"));
inline const Value simulated = Value(std::string("SIMULATED"));
inline const Value angularvelocityunit = Value(std::string("ANGULARVELOCITYUNIT"));
inline const Value compoundplaneangleunit = Value(std::string("COMPOUNDPLANEANGLEUNIT"));
inline const Value dynamicviscosityunit = Value(std::string("DYNAMICVISCOSITYUNIT"));
inline const Value heatfluxdensityunit = Value(std::string("HEATFLUXDENSITYUNIT"));
inline const Value integercountrateunit = Value(std::string("INTEGERCOUNTRATEUNIT"));
inline const Value isothermalmoisturecapacityunit = Value(std::string("ISOTHERMALMOISTURECAPACITYUNIT"));
inline const Value kinematicviscosityunit = Value(std::string("KINEMATICVISCOSITYUNIT"));
inline const Value linearvelocityunit = Value(std::string("LINEARVELOCITYUNIT"));
inline const Value massdensityunit = Value(std::string("MASSDENSITYUNIT"));
inline const Value massflowrateunit = Value(std::string("MASSFLOWRATEUNIT"));
inline const Value moisturediffusivityunit = Value(std::string("MOISTUREDIFFUSIVITYUNIT"));
inline const Value molecularweightunit = Value(std::string("MOLECULARWEIGHTUNIT"));
inline const Value specificheatcapacityunit = Value(std::string("SPECIFICHEATCAPACITYUNIT"));
inline const Value thermaladmittanceunit = Value(std::string("THERMALADMITTANCEUNIT"));
inline const Value thermalconductanceunit = Value(std::string("THERMALCONDUCTANCEUNIT"));
inline const Value thermalresistanceunit = Value(std::string("THERMALRESISTANCEUNIT"));
inline const Value thermaltransmittanceunit = Value(std::string("THERMALTRANSMITTANCEUNIT"));
inline const Value vaporpermeabilityunit = Value(std::string("VAPORPERMEABILITYUNIT"));
inline const Value volumetricflowrateunit = Value(std::string("VOLUMETRICFLOWRATEUNIT"));
inline const Value rotationalfrequencyunit = Value(std::string("ROTATIONALFREQUENCYUNIT"));
inline const Value torqueunit = Value(std::string("TORQUEUNIT"));
inline const Value momentofinertiaunit = Value(std::string("MOMENTOFINERTIAUNIT"));
inline const Value linearmomentunit = Value(std::string("LINEARMOMENTUNIT"));
inline const Value linearforceunit = Value(std::string("LINEARFORCEUNIT"));
inline const Value planarforceunit = Value(std::string("PLANARFORCEUNIT"));
inline const Value modulusofelasticityunit = Value(std::string("MODULUSOFELASTICITYUNIT"));
inline const Value shearmodulusunit = Value(std::string("SHEARMODULUSUNIT"));
inline const Value linearstiffnessunit = Value(std::string("LINEARSTIFFNESSUNIT"));
inline const Value rotationalstiffnessunit = Value(std::string("ROTATIONALSTIFFNESSUNIT"));
inline const Value modulusofsubgradereactionunit = Value(std::string("MODULUSOFSUBGRADEREACTIONUNIT"));
inline const Value accelerationunit = Value(std::string("ACCELERATIONUNIT"));
inline const Value curvatureunit = Value(std::string("CURVATUREUNIT"));
inline const Value heatingvalueunit = Value(std::string("HEATINGVALUEUNIT"));
inline const Value ionconcentrationunit = Value(std::string("IONCONCENTRATIONUNIT"));
inline const Value luminousintensitydistributionunit = Value(std::string("LUMINOUSINTENSITYDISTRIBUTIONUNIT"));
inline const Value massperlengthunit = Value(std::string("MASSPERLENGTHUNIT"));
inline const Value modulusoflinearsubgradereactionunit = Value(std::string("MODULUSOFLINEARSUBGRADEREACTIONUNIT"));
inline const Value modulusofrotationalsubgradereactionunit = Value(std::string("MODULUSOFROTATIONALSUBGRADEREACTIONUNIT"));
inline const Value phunit = Value(std::string("PHUNIT"));
inline const Value rotationalmassunit = Value(std::string("ROTATIONALMASSUNIT"));
inline const Value sectionareaintegralunit = Value(std::string("SECTIONAREAINTEGRALUNIT"));
inline const Value sectionmodulusunit = Value(std::string("SECTIONMODULUSUNIT"));
inline const Value soundpowerunit = Value(std::string("SOUNDPOWERUNIT"));
inline const Value soundpressureunit = Value(std::string("SOUNDPRESSUREUNIT"));
inline const Value temperaturegradientunit = Value(std::string("TEMPERATUREGRADIENTUNIT"));
inline const Value thermalexpansioncoefficientunit = Value(std::string("THERMALEXPANSIONCOEFFICIENTUNIT"));
inline const Value warpingconstantunit = Value(std::string("WARPINGCONSTANTUNIT"));
inline const Value warpingmomentunit = Value(std::string("WARPINGMOMENTUNIT"));
inline const Value origin = Value(std::string("ORIGIN"));
inline const Value target = Value(std::string("TARGET"));
inline const Value positive = Value(std::string("POSITIVE"));
inline const Value negative = Value(std::string("NEGATIVE"));
inline const Value formedduct = Value(std::string("FORMEDDUCT"));
inline const Value inspectionchamber = Value(std::string("INSPECTIONCHAMBER"));
inline const Value inspectionpit = Value(std::string("INSPECTIONPIT"));
inline const Value manhole = Value(std::string("MANHOLE"));
inline const Value meterchamber = Value(std::string("METERCHAMBER"));
inline const Value sump = Value(std::string("SUMP"));
inline const Value trench = Value(std::string("TRENCH"));
inline const Value valvechamber = Value(std::string("VALVECHAMBER"));
inline const Value public_ = Value(std::string("PUBLIC"));
inline const Value restricted = Value(std::string("RESTRICTED"));
inline const Value confidential = Value(std::string("CONFIDENTIAL"));
inline const Value personal = Value(std::string("PERSONAL"));
inline const Value draft = Value(std::string("DRAFT"));
inline const Value finaldraft = Value(std::string("FINALDRAFT"));
inline const Value final = Value(std::string("FINAL"));
inline const Value revision = Value(std::string("REVISION"));
inline const Value swinging = Value(std::string("SWINGING"));
inline const Value double_acting = Value(std::string("DOUBLE_ACTING"));
inline const Value sliding = Value(std::string("SLIDING"));
inline const Value folding = Value(std::string("FOLDING"));
inline const Value revolving = Value(std::string("REVOLVING"));
inline const Value rollingup = Value(std::string("ROLLINGUP"));
inline const Value left = Value(std::string("LEFT"));
inline const Value middle = Value(std::string("MIDDLE"));
inline const Value right = Value(std::string("RIGHT"));
inline const Value aluminium = Value(std::string("ALUMINIUM"));
inline const Value high_grade_steel = Value(std::string("HIGH_GRADE_STEEL"));
inline const Value steel = Value(std::string("STEEL"));
inline const Value wood = Value(std::string("WOOD"));
inline const Value aluminium_wood = Value(std::string("ALUMINIUM_WOOD"));
inline const Value aluminium_plastic = Value(std::string("ALUMINIUM_PLASTIC"));
inline const Value plastic = Value(std::string("PLASTIC"));
inline const Value single_swing_left = Value(std::string("SINGLE_SWING_LEFT"));
inline const Value single_swing_right = Value(std::string("SINGLE_SWING_RIGHT"));
inline const Value double_door_single_swing = Value(std::string("DOUBLE_DOOR_SINGLE_SWING"));
inline const Value double_door_single_swing_opposite_left = Value(std::string("DOUBLE_DOOR_SINGLE_SWING_OPPOSITE_LEFT"));
inline const Value double_door_single_swing_opposite_right = Value(std::string("DOUBLE_DOOR_SINGLE_SWING_OPPOSITE_RIGHT"));
inline const Value double_swing_left = Value(std::string("DOUBLE_SWING_LEFT"));
inline const Value double_swing_right = Value(std::string("DOUBLE_SWING_RIGHT"));
inline const Value double_door_double_swing = Value(std::string("DOUBLE_DOOR_DOUBLE_SWING"));
inline const Value sliding_to_left = Value(std::string("SLIDING_TO_LEFT"));
inline const Value sliding_to_right = Value(std::string("SLIDING_TO_RIGHT"));
inline const Value double_door_sliding = Value(std::string("DOUBLE_DOOR_SLIDING"));
inline const Value folding_to_left = Value(std::string("FOLDING_TO_LEFT"));
inline const Value folding_to_right = Value(std::string("FOLDING_TO_RIGHT"));
inline const Value double_door_folding = Value(std::string("DOUBLE_DOOR_FOLDING"));
inline const Value connector = Value(std::string("CONNECTOR"));
inline const Value entry = Value(std::string("ENTRY"));
inline const Value exit = Value(std::string("EXIT"));
inline const Value junction = Value(std::string("JUNCTION"));
inline const Value obstruction = Value(std::string("OBSTRUCTION"));
inline const Value transition = Value(std::string("TRANSITION"));
inline const Value rigidsegment = Value(std::string("RIGIDSEGMENT"));
inline const Value flexiblesegment = Value(std::string("FLEXIBLESEGMENT"));
inline const Value flatoval = Value(std::string("FLATOVAL"));
inline const Value rectangular = Value(std::string("RECTANGULAR"));
inline const Value round = Value(std::string("ROUND"));
inline const Value computer = Value(std::string("COMPUTER"));
inline const Value directwaterheater = Value(std::string("DIRECTWATERHEATER"));
inline const Value dishwasher = Value(std::string("DISHWASHER"));
inline const Value electriccooker = Value(std::string("ELECTRICCOOKER"));
inline const Value electricheater = Value(std::string("ELECTRICHEATER"));
inline const Value facsimile = Value(std::string("FACSIMILE"));
inline const Value freestandingfan = Value(std::string("FREESTANDINGFAN"));
inline const Value freezer = Value(std::string("FREEZER"));
inline const Value fridge_freezer = Value(std::string("FRIDGE_FREEZER"));
inline const Value handdryer = Value(std::string("HANDDRYER"));
inline const Value indirectwaterheater = Value(std::string("INDIRECTWATERHEATER"));
inline const Value microwave = Value(std::string("MICROWAVE"));
inline const Value photocopier = Value(std::string("PHOTOCOPIER"));
inline const Value printer = Value(std::string("PRINTER"));
inline const Value refrigerator = Value(std::string("REFRIGERATOR"));
inline const Value radiantheater = Value(std::string("RADIANTHEATER"));
inline const Value scanner = Value(std::string("SCANNER"));
inline const Value telephone = Value(std::string("TELEPHONE"));
inline const Value tumbledryer = Value(std::string("TUMBLEDRYER"));
inline const Value tv = Value(std::string("TV"));
inline const Value vendingmachine = Value(std::string("VENDINGMACHINE"));
inline const Value washingmachine = Value(std::string("WASHINGMACHINE"));
inline const Value waterheater = Value(std::string("WATERHEATER"));
inline const Value watercooler = Value(std::string("WATERCOOLER"));
inline const Value alternating = Value(std::string("ALTERNATING"));
inline const Value direct = Value(std::string("DIRECT"));
inline const Value alarmpanel = Value(std::string("ALARMPANEL"));
inline const Value consumerunit = Value(std::string("CONSUMERUNIT"));
inline const Value controlpanel = Value(std::string("CONTROLPANEL"));
inline const Value distributionboard = Value(std::string("DISTRIBUTIONBOARD"));
inline const Value gasdetectorpanel = Value(std::string("GASDETECTORPANEL"));
inline const Value indicatorpanel = Value(std::string("INDICATORPANEL"));
inline const Value mimicpanel = Value(std::string("MIMICPANEL"));
inline const Value motorcontrolcentre = Value(std::string("MOTORCONTROLCENTRE"));
inline const Value switchboard = Value(std::string("SWITCHBOARD"));
inline const Value battery = Value(std::string("BATTERY"));
inline const Value capacitorbank = Value(std::string("CAPACITORBANK"));
inline const Value harmonicfilter = Value(std::string("HARMONICFILTER"));
inline const Value inductorbank = Value(std::string("INDUCTORBANK"));
inline const Value ups = Value(std::string("UPS"));
inline const Value electricpointheater = Value(std::string("ELECTRICPOINTHEATER"));
inline const Value electriccableheater = Value(std::string("ELECTRICCABLEHEATER"));
inline const Value electricmatheater = Value(std::string("ELECTRICMATHEATER"));
inline const Value dc = Value(std::string("DC"));
inline const Value induction = Value(std::string("INDUCTION"));
inline const Value polyphase = Value(std::string("POLYPHASE"));
inline const Value reluctancesynchronous = Value(std::string("RELUCTANCESYNCHRONOUS"));
inline const Value synchronous = Value(std::string("SYNCHRONOUS"));
inline const Value timeclock = Value(std::string("TIMECLOCK"));
inline const Value timedelay = Value(std::string("TIMEDELAY"));
inline const Value relay = Value(std::string("RELAY"));
inline const Value accessory_assembly = Value(std::string("ACCESSORY_ASSEMBLY"));
inline const Value arch = Value(std::string("ARCH"));
inline const Value beam_grid = Value(std::string("BEAM_GRID"));
inline const Value braced_frame = Value(std::string("BRACED_FRAME"));
inline const Value girder = Value(std::string("GIRDER"));
inline const Value reinforcement_unit = Value(std::string("REINFORCEMENT_UNIT"));
inline const Value rigid_frame = Value(std::string("RIGID_FRAME"));
inline const Value slab_field = Value(std::string("SLAB_FIELD"));
inline const Value truss = Value(std::string("TRUSS"));
inline const Value complex = Value(std::string("COMPLEX"));
inline const Value element = Value(std::string("ELEMENT"));
inline const Value partial = Value(std::string("PARTIAL"));
inline const Value primary = Value(std::string("PRIMARY"));
inline const Value secondary = Value(std::string("SECONDARY"));
inline const Value tertiary = Value(std::string("TERTIARY"));
inline const Value auxiliary = Value(std::string("AUXILIARY"));
inline const Value combinedvalue = Value(std::string("COMBINEDVALUE"));
inline const Value disposal = Value(std::string("DISPOSAL"));
inline const Value extraction = Value(std::string("EXTRACTION"));
inline const Value installation = Value(std::string("INSTALLATION"));
inline const Value manufacture = Value(std::string("MANUFACTURE"));
inline const Value transportation = Value(std::string("TRANSPORTATION"));
inline const Value directevaporativerandommediaaircooler = Value(std::string("DIRECTEVAPORATIVERANDOMMEDIAAIRCOOLER"));
inline const Value directevaporativerigidmediaaircooler = Value(std::string("DIRECTEVAPORATIVERIGIDMEDIAAIRCOOLER"));
inline const Value directevaporativeslingerspackagedaircooler = Value(std::string("DIRECTEVAPORATIVESLINGERSPACKAGEDAIRCOOLER"));
inline const Value directevaporativepackagedrotaryaircooler = Value(std::string("DIRECTEVAPORATIVEPACKAGEDROTARYAIRCOOLER"));
inline const Value directevaporativeairwasher = Value(std::string("DIRECTEVAPORATIVEAIRWASHER"));
inline const Value indirectevaporativepackageaircooler = Value(std::string("INDIRECTEVAPORATIVEPACKAGEAIRCOOLER"));
inline const Value indirectevaporativewetcoil = Value(std::string("INDIRECTEVAPORATIVEWETCOIL"));
inline const Value indirectevaporativecoolingtowerorcoilcooler = Value(std::string("INDIRECTEVAPORATIVECOOLINGTOWERORCOILCOOLER"));
inline const Value indirectdirectcombination = Value(std::string("INDIRECTDIRECTCOMBINATION"));
inline const Value directexpansionshellandtube = Value(std::string("DIRECTEXPANSIONSHELLANDTUBE"));
inline const Value directexpansiontubeintube = Value(std::string("DIRECTEXPANSIONTUBEINTUBE"));
inline const Value directexpansionbrazedplate = Value(std::string("DIRECTEXPANSIONBRAZEDPLATE"));
inline const Value floodedshellandtube = Value(std::string("FLOODEDSHELLANDTUBE"));
inline const Value shellandcoil = Value(std::string("SHELLANDCOIL"));
inline const Value centrifugalforwardcurved = Value(std::string("CENTRIFUGALFORWARDCURVED"));
inline const Value centrifugalradial = Value(std::string("CENTRIFUGALRADIAL"));
inline const Value centrifugalbackwardinclinedcurved = Value(std::string("CENTRIFUGALBACKWARDINCLINEDCURVED"));
inline const Value centrifugalairfoil = Value(std::string("CENTRIFUGALAIRFOIL"));
inline const Value tubeaxial = Value(std::string("TUBEAXIAL"));
inline const Value vaneaxial = Value(std::string("VANEAXIAL"));
inline const Value propelloraxial = Value(std::string("PROPELLORAXIAL"));
inline const Value airparticlefilter = Value(std::string("AIRPARTICLEFILTER"));
inline const Value odorfilter = Value(std::string("ODORFILTER"));
inline const Value oilfilter = Value(std::string("OILFILTER"));
inline const Value strainer = Value(std::string("STRAINER"));
inline const Value waterfilter = Value(std::string("WATERFILTER"));
inline const Value breechinginlet = Value(std::string("BREECHINGINLET"));
inline const Value firehydrant = Value(std::string("FIREHYDRANT"));
inline const Value hosereel = Value(std::string("HOSEREEL"));
inline const Value sprinkler = Value(std::string("SPRINKLER"));
inline const Value sprinklerdeflector = Value(std::string("SPRINKLERDEFLECTOR"));
inline const Value source = Value(std::string("SOURCE"));
inline const Value sink = Value(std::string("SINK"));
inline const Value sourceandsink = Value(std::string("SOURCEANDSINK"));
inline const Value pressuregauge = Value(std::string("PRESSUREGAUGE"));
inline const Value thermometer = Value(std::string("THERMOMETER"));
inline const Value ammeter = Value(std::string("AMMETER"));
inline const Value frequencymeter = Value(std::string("FREQUENCYMETER"));
inline const Value powerfactormeter = Value(std::string("POWERFACTORMETER"));
inline const Value phaseanglemeter = Value(std::string("PHASEANGLEMETER"));
inline const Value voltmeter_peak = Value(std::string("VOLTMETER_PEAK"));
inline const Value voltmeter_rms = Value(std::string("VOLTMETER_RMS"));
inline const Value electricmeter = Value(std::string("ELECTRICMETER"));
inline const Value energymeter = Value(std::string("ENERGYMETER"));
inline const Value flowmeter = Value(std::string("FLOWMETER"));
inline const Value gasmeter = Value(std::string("GASMETER"));
inline const Value oilmeter = Value(std::string("OILMETER"));
inline const Value watermeter = Value(std::string("WATERMETER"));
inline const Value footing_beam = Value(std::string("FOOTING_BEAM"));
inline const Value pad_footing = Value(std::string("PAD_FOOTING"));
inline const Value pile_cap = Value(std::string("PILE_CAP"));
inline const Value strip_footing = Value(std::string("STRIP_FOOTING"));
inline const Value gasappliance = Value(std::string("GASAPPLIANCE"));
inline const Value gasbooster = Value(std::string("GASBOOSTER"));
inline const Value gasburner = Value(std::string("GASBURNER"));
inline const Value graph_view = Value(std::string("GRAPH_VIEW"));
inline const Value sketch_view = Value(std::string("SKETCH_VIEW"));
inline const Value model_view = Value(std::string("MODEL_VIEW"));
inline const Value plan_view = Value(std::string("PLAN_VIEW"));
inline const Value reflected_plan_view = Value(std::string("REFLECTED_PLAN_VIEW"));
inline const Value section_view = Value(std::string("SECTION_VIEW"));
inline const Value elevation_view = Value(std::string("ELEVATION_VIEW"));
inline const Value global_coords = Value(std::string("GLOBAL_COORDS"));
inline const Value local_coords = Value(std::string("LOCAL_COORDS"));
inline const Value plate = Value(std::string("PLATE"));
inline const Value shellandtube = Value(std::string("SHELLANDTUBE"));
inline const Value steaminjection = Value(std::string("STEAMINJECTION"));
inline const Value adiabaticairwasher = Value(std::string("ADIABATICAIRWASHER"));
inline const Value adiabaticpan = Value(std::string("ADIABATICPAN"));
inline const Value adiabaticwettedelement = Value(std::string("ADIABATICWETTEDELEMENT"));
inline const Value adiabaticatomizing = Value(std::string("ADIABATICATOMIZING"));
inline const Value adiabaticultrasonic = Value(std::string("ADIABATICULTRASONIC"));
inline const Value adiabaticrigidmedia = Value(std::string("ADIABATICRIGIDMEDIA"));
inline const Value adiabaticcompressedairnozzle = Value(std::string("ADIABATICCOMPRESSEDAIRNOZZLE"));
inline const Value assistedelectric = Value(std::string("ASSISTEDELECTRIC"));
inline const Value assistednaturalgas = Value(std::string("ASSISTEDNATURALGAS"));
inline const Value assistedpropane = Value(std::string("ASSISTEDPROPANE"));
inline const Value assistedbutane = Value(std::string("ASSISTEDBUTANE"));
inline const Value assistedsteam = Value(std::string("ASSISTEDSTEAM"));
inline const Value internal = Value(std::string("INTERNAL"));
inline const Value external = Value(std::string("EXTERNAL"));
inline const Value assetinventory = Value(std::string("ASSETINVENTORY"));
inline const Value spaceinventory = Value(std::string("SPACEINVENTORY"));
inline const Value furnitureinventory = Value(std::string("FURNITUREINVENTORY"));
inline const Value compactfluorescent = Value(std::string("COMPACTFLUORESCENT"));
inline const Value fluorescent = Value(std::string("FLUORESCENT"));
inline const Value highpressuremercury = Value(std::string("HIGHPRESSUREMERCURY"));
inline const Value highpressuresodium = Value(std::string("HIGHPRESSURESODIUM"));
inline const Value metalhalide = Value(std::string("METALHALIDE"));
inline const Value tungstenfilament = Value(std::string("TUNGSTENFILAMENT"));
inline const Value axis1 = Value(std::string("AXIS1"));
inline const Value axis2 = Value(std::string("AXIS2"));
inline const Value axis3 = Value(std::string("AXIS3"));
inline const Value type_a = Value(std::string("TYPE_A"));
inline const Value type_b = Value(std::string("TYPE_B"));
inline const Value type_c = Value(std::string("TYPE_C"));
inline const Value lightemittingdiode = Value(std::string("LIGHTEMITTINGDIODE"));
inline const Value lowpressuresodium = Value(std::string("LOWPRESSURESODIUM"));
inline const Value lowvoltagehalogen = Value(std::string("LOWVOLTAGEHALOGEN"));
inline const Value mainvoltagehalogen = Value(std::string("MAINVOLTAGEHALOGEN"));
inline const Value pointsource = Value(std::string("POINTSOURCE"));
inline const Value directionsource = Value(std::string("DIRECTIONSOURCE"));
inline const Value load_group = Value(std::string("LOAD_GROUP"));
inline const Value load_case = Value(std::string("LOAD_CASE"));
inline const Value load_combination_group = Value(std::string("LOAD_COMBINATION_GROUP"));
inline const Value load_combination = Value(std::string("LOAD_COMBINATION"));
inline const Value logicaland = Value(std::string("LOGICALAND"));
inline const Value logicalor = Value(std::string("LOGICALOR"));
inline const Value brace = Value(std::string("BRACE"));
inline const Value chord = Value(std::string("CHORD"));
inline const Value collar = Value(std::string("COLLAR"));
inline const Value member = Value(std::string("MEMBER"));
inline const Value mullion = Value(std::string("MULLION"));
inline const Value post = Value(std::string("POST"));
inline const Value purlin = Value(std::string("PURLIN"));
inline const Value rafter = Value(std::string("RAFTER"));
inline const Value stringer = Value(std::string("STRINGER"));
inline const Value strut = Value(std::string("STRUT"));
inline const Value stud = Value(std::string("STUD"));
inline const Value beltdrive = Value(std::string("BELTDRIVE"));
inline const Value coupling = Value(std::string("COUPLING"));
inline const Value directdrive = Value(std::string("DIRECTDRIVE"));
inline const Value null = Value(std::string("NULL"));
inline const Value product = Value(std::string("PRODUCT"));
inline const Value process = Value(std::string("PROCESS"));
inline const Value control = Value(std::string("CONTROL"));
inline const Value resource = Value(std::string("RESOURCE"));
inline const Value actor = Value(std::string("ACTOR"));
inline const Value group = Value(std::string("GROUP"));
inline const Value project = Value(std::string("PROJECT"));
inline const Value codecompliance = Value(std::string("CODECOMPLIANCE"));
inline const Value designintent = Value(std::string("DESIGNINTENT"));
inline const Value healthandsafety = Value(std::string("HEALTHANDSAFETY"));
inline const Value requirement = Value(std::string("REQUIREMENT"));
inline const Value specification = Value(std::string("SPECIFICATION"));
inline const Value triggercondition = Value(std::string("TRIGGERCONDITION"));
inline const Value assignee = Value(std::string("ASSIGNEE"));
inline const Value assignor = Value(std::string("ASSIGNOR"));
inline const Value lessee = Value(std::string("LESSEE"));
inline const Value lessor = Value(std::string("LESSOR"));
inline const Value lettingagent = Value(std::string("LETTINGAGENT"));
inline const Value owner = Value(std::string("OWNER"));
inline const Value tenant = Value(std::string("TENANT"));
inline const Value audiovisualoutlet = Value(std::string("AUDIOVISUALOUTLET"));
inline const Value communicationsoutlet = Value(std::string("COMMUNICATIONSOUTLET"));
inline const Value poweroutlet = Value(std::string("POWEROUTLET"));
inline const Value grill = Value(std::string("GRILL"));
inline const Value louver = Value(std::string("LOUVER"));
inline const Value screen = Value(std::string("SCREEN"));
inline const Value physical = Value(std::string("PHYSICAL"));
inline const Value virtual_ = Value(std::string("VIRTUAL"));
inline const Value cast_in_place = Value(std::string("CAST_IN_PLACE"));
inline const Value composite = Value(std::string("COMPOSITE"));
inline const Value precast_concrete = Value(std::string("PRECAST_CONCRETE"));
inline const Value prefab_steel = Value(std::string("PREFAB_STEEL"));
inline const Value cohesion = Value(std::string("COHESION"));
inline const Value friction = Value(std::string("FRICTION"));
inline const Value support = Value(std::string("SUPPORT"));
inline const Value gutter = Value(std::string("GUTTER"));
inline const Value spool = Value(std::string("SPOOL"));
inline const Value curtain_panel = Value(std::string("CURTAIN_PANEL"));
inline const Value sheet = Value(std::string("SHEET"));
inline const Value advice_caution = Value(std::string("ADVICE_CAUTION"));
inline const Value advice_note = Value(std::string("ADVICE_NOTE"));
inline const Value advice_warning = Value(std::string("ADVICE_WARNING"));
inline const Value calibration = Value(std::string("CALIBRATION"));
inline const Value diagnostic = Value(std::string("DIAGNOSTIC"));
inline const Value shutdown = Value(std::string("SHUTDOWN"));
inline const Value startup = Value(std::string("STARTUP"));
inline const Value curve = Value(std::string("CURVE"));
inline const Value area = Value(std::string("AREA"));
inline const Value change = Value(std::string("CHANGE"));
inline const Value maintenance = Value(std::string("MAINTENANCE"));
inline const Value move = Value(std::string("MOVE"));
inline const Value purchase = Value(std::string("PURCHASE"));
inline const Value work = Value(std::string("WORK"));
inline const Value changeorder = Value(std::string("CHANGEORDER"));
inline const Value maintenanceworkorder = Value(std::string("MAINTENANCEWORKORDER"));
inline const Value moveorder = Value(std::string("MOVEORDER"));
inline const Value purchaseorder = Value(std::string("PURCHASEORDER"));
inline const Value workorder = Value(std::string("WORKORDER"));
inline const Value projected_length = Value(std::string("PROJECTED_LENGTH"));
inline const Value true_length = Value(std::string("TRUE_LENGTH"));
inline const Value design = Value(std::string("DESIGN"));
inline const Value designmaximum = Value(std::string("DESIGNMAXIMUM"));
inline const Value designminimum = Value(std::string("DESIGNMINIMUM"));
inline const Value asbuilt = Value(std::string("ASBUILT"));
inline const Value commissioning = Value(std::string("COMMISSIONING"));
inline const Value notknown = Value(std::string("NOTKNOWN"));
inline const Value fusedisconnector = Value(std::string("FUSEDISCONNECTOR"));
inline const Value circuitbreaker = Value(std::string("CIRCUITBREAKER"));
inline const Value earthfailuredevice = Value(std::string("EARTHFAILUREDEVICE"));
inline const Value residualcurrentcircuitbreaker = Value(std::string("RESIDUALCURRENTCIRCUITBREAKER"));
inline const Value residualcurrentswitch = Value(std::string("RESIDUALCURRENTSWITCH"));
inline const Value varistor = Value(std::string("VARISTOR"));
inline const Value circulator = Value(std::string("CIRCULATOR"));
inline const Value endsuction = Value(std::string("ENDSUCTION"));
inline const Value splitcase = Value(std::string("SPLITCASE"));
inline const Value verticalinline = Value(std::string("VERTICALINLINE"));
inline const Value verticalturbine = Value(std::string("VERTICALTURBINE"));
inline const Value handrail = Value(std::string("HANDRAIL"));
inline const Value guardrail = Value(std::string("GUARDRAIL"));
inline const Value balustrade = Value(std::string("BALUSTRADE"));
inline const Value straight = Value(std::string("STRAIGHT"));
inline const Value spiral = Value(std::string("SPIRAL"));
inline const Value straight_run_ramp = Value(std::string("STRAIGHT_RUN_RAMP"));
inline const Value two_straight_run_ramp = Value(std::string("TWO_STRAIGHT_RUN_RAMP"));
inline const Value quarter_turn_ramp = Value(std::string("QUARTER_TURN_RAMP"));
inline const Value two_quarter_turn_ramp = Value(std::string("TWO_QUARTER_TURN_RAMP"));
inline const Value half_turn_ramp = Value(std::string("HALF_TURN_RAMP"));
inline const Value spiral_ramp = Value(std::string("SPIRAL_RAMP"));
inline const Value blinn = Value(std::string("BLINN"));
inline const Value flat = Value(std::string("FLAT"));
inline const Value glass = Value(std::string("GLASS"));
inline const Value matt = Value(std::string("MATT"));
inline const Value metal = Value(std::string("METAL"));
inline const Value mirror = Value(std::string("MIRROR"));
inline const Value phong = Value(std::string("PHONG"));
inline const Value strauss = Value(std::string("STRAUSS"));
inline const Value main = Value(std::string("MAIN"));
inline const Value shear = Value(std::string("SHEAR"));
inline const Value ligature = Value(std::string("LIGATURE"));
inline const Value punching = Value(std::string("PUNCHING"));
inline const Value edge = Value(std::string("EDGE"));
inline const Value ring = Value(std::string("RING"));
inline const Value plain = Value(std::string("PLAIN"));
inline const Value textured = Value(std::string("TEXTURED"));
inline const Value consumed = Value(std::string("CONSUMED"));
inline const Value partiallyconsumed = Value(std::string("PARTIALLYCONSUMED"));
inline const Value notconsumed = Value(std::string("NOTCONSUMED"));
inline const Value occupied = Value(std::string("OCCUPIED"));
inline const Value partiallyoccupied = Value(std::string("PARTIALLYOCCUPIED"));
inline const Value notoccupied = Value(std::string("NOTOCCUPIED"));
inline const Value direction_x = Value(std::string("DIRECTION_X"));
inline const Value direction_y = Value(std::string("DIRECTION_Y"));
inline const Value supplier = Value(std::string("SUPPLIER"));
inline const Value manufacturer = Value(std::string("MANUFACTURER"));
inline const Value contractor = Value(std::string("CONTRACTOR"));
inline const Value subcontractor = Value(std::string("SUBCONTRACTOR"));
inline const Value architect = Value(std::string("ARCHITECT"));
inline const Value structuralengineer = Value(std::string("STRUCTURALENGINEER"));
inline const Value costengineer = Value(std::string("COSTENGINEER"));
inline const Value client = Value(std::string("CLIENT"));
inline const Value buildingowner = Value(std::string("BUILDINGOWNER"));
inline const Value buildingoperator = Value(std::string("BUILDINGOPERATOR"));
inline const Value mechanicalengineer = Value(std::string("MECHANICALENGINEER"));
inline const Value electricalengineer = Value(std::string("ELECTRICALENGINEER"));
inline const Value projectmanager = Value(std::string("PROJECTMANAGER"));
inline const Value facilitiesmanager = Value(std::string("FACILITIESMANAGER"));
inline const Value civilengineer = Value(std::string("CIVILENGINEER"));
inline const Value comissioningengineer = Value(std::string("COMISSIONINGENGINEER"));
inline const Value engineer = Value(std::string("ENGINEER"));
inline const Value consultant = Value(std::string("CONSULTANT"));
inline const Value constructionmanager = Value(std::string("CONSTRUCTIONMANAGER"));
inline const Value fieldconstructionmanager = Value(std::string("FIELDCONSTRUCTIONMANAGER"));
inline const Value reseller = Value(std::string("RESELLER"));
inline const Value flat_roof = Value(std::string("FLAT_ROOF"));
inline const Value shed_roof = Value(std::string("SHED_ROOF"));
inline const Value gable_roof = Value(std::string("GABLE_ROOF"));
inline const Value hip_roof = Value(std::string("HIP_ROOF"));
inline const Value hipped_gable_roof = Value(std::string("HIPPED_GABLE_ROOF"));
inline const Value gambrel_roof = Value(std::string("GAMBREL_ROOF"));
inline const Value mansard_roof = Value(std::string("MANSARD_ROOF"));
inline const Value barrel_roof = Value(std::string("BARREL_ROOF"));
inline const Value rainbow_roof = Value(std::string("RAINBOW_ROOF"));
inline const Value butterfly_roof = Value(std::string("BUTTERFLY_ROOF"));
inline const Value pavilion_roof = Value(std::string("PAVILION_ROOF"));
inline const Value dome_roof = Value(std::string("DOME_ROOF"));
inline const Value freeform = Value(std::string("FREEFORM"));
inline const Value exa = Value(std::string("EXA"));
inline const Value peta = Value(std::string("PETA"));
inline const Value tera = Value(std::string("TERA"));
inline const Value giga = Value(std::string("GIGA"));
inline const Value mega = Value(std::string("MEGA"));
inline const Value kilo = Value(std::string("KILO"));
inline const Value hecto = Value(std::string("HECTO"));
inline const Value deca = Value(std::string("DECA"));
inline const Value deci = Value(std::string("DECI"));
inline const Value centi = Value(std::string("CENTI"));
inline const Value milli = Value(std::string("MILLI"));
inline const Value micro = Value(std::string("MICRO"));
inline const Value nano = Value(std::string("NANO"));
inline const Value pico = Value(std::string("PICO"));
inline const Value femto = Value(std::string("FEMTO"));
inline const Value atto = Value(std::string("ATTO"));
inline const Value ampere = Value(std::string("AMPERE"));
inline const Value becquerel = Value(std::string("BECQUEREL"));
inline const Value candela = Value(std::string("CANDELA"));
inline const Value coulomb = Value(std::string("COULOMB"));
inline const Value cubic_metre = Value(std::string("CUBIC_METRE"));
inline const Value degree_celsius = Value(std::string("DEGREE_CELSIUS"));
inline const Value farad = Value(std::string("FARAD"));
inline const Value gram = Value(std::string("GRAM"));
inline const Value gray = Value(std::string("GRAY"));
inline const Value henry = Value(std::string("HENRY"));
inline const Value hertz = Value(std::string("HERTZ"));
inline const Value joule = Value(std::string("JOULE"));
inline const Value kelvin = Value(std::string("KELVIN"));
inline const Value lumen = Value(std::string("LUMEN"));
inline const Value lux = Value(std::string("LUX"));
inline const Value metre = Value(std::string("METRE"));
inline const Value mole = Value(std::string("MOLE"));
inline const Value newton = Value(std::string("NEWTON"));
inline const Value ohm = Value(std::string("OHM"));
inline const Value pascal = Value(std::string("PASCAL"));
inline const Value radian = Value(std::string("RADIAN"));
inline const Value second = Value(std::string("SECOND"));
inline const Value siemens = Value(std::string("SIEMENS"));
inline const Value sievert = Value(std::string("SIEVERT"));
inline const Value square_metre = Value(std::string("SQUARE_METRE"));
inline const Value steradian = Value(std::string("STERADIAN"));
inline const Value tesla = Value(std::string("TESLA"));
inline const Value volt = Value(std::string("VOLT"));
inline const Value watt = Value(std::string("WATT"));
inline const Value weber = Value(std::string("WEBER"));
inline const Value bath = Value(std::string("BATH"));
inline const Value bidet = Value(std::string("BIDET"));
inline const Value cistern = Value(std::string("CISTERN"));
inline const Value shower = Value(std::string("SHOWER"));
inline const Value sanitaryfountain = Value(std::string("SANITARYFOUNTAIN"));
inline const Value toiletpan = Value(std::string("TOILETPAN"));
inline const Value urinal = Value(std::string("URINAL"));
inline const Value washhandbasin = Value(std::string("WASHHANDBASIN"));
inline const Value wcseat = Value(std::string("WCSEAT"));
inline const Value uniform = Value(std::string("UNIFORM"));
inline const Value tapered = Value(std::string("TAPERED"));
inline const Value co2sensor = Value(std::string("CO2SENSOR"));
inline const Value firesensor = Value(std::string("FIRESENSOR"));
inline const Value flowsensor = Value(std::string("FLOWSENSOR"));
inline const Value gassensor = Value(std::string("GASSENSOR"));
inline const Value heatsensor = Value(std::string("HEATSENSOR"));
inline const Value humiditysensor = Value(std::string("HUMIDITYSENSOR"));
inline const Value lightsensor = Value(std::string("LIGHTSENSOR"));
inline const Value moisturesensor = Value(std::string("MOISTURESENSOR"));
inline const Value movementsensor = Value(std::string("MOVEMENTSENSOR"));
inline const Value pressuresensor = Value(std::string("PRESSURESENSOR"));
inline const Value smokesensor = Value(std::string("SMOKESENSOR"));
inline const Value soundsensor = Value(std::string("SOUNDSENSOR"));
inline const Value temperaturesensor = Value(std::string("TEMPERATURESENSOR"));
inline const Value start_start = Value(std::string("START_START"));
inline const Value start_finish = Value(std::string("START_FINISH"));
inline const Value finish_start = Value(std::string("FINISH_START"));
inline const Value finish_finish = Value(std::string("FINISH_FINISH"));
inline const Value a_qualityofcomponents = Value(std::string("A_QUALITYOFCOMPONENTS"));
inline const Value b_designlevel = Value(std::string("B_DESIGNLEVEL"));
inline const Value c_workexecutionlevel = Value(std::string("C_WORKEXECUTIONLEVEL"));
inline const Value d_indoorenvironment = Value(std::string("D_INDOORENVIRONMENT"));
inline const Value e_outdoorenvironment = Value(std::string("E_OUTDOORENVIRONMENT"));
inline const Value f_inuseconditions = Value(std::string("F_INUSECONDITIONS"));
inline const Value g_maintenancelevel = Value(std::string("G_MAINTENANCELEVEL"));
inline const Value actualservicelife = Value(std::string("ACTUALSERVICELIFE"));
inline const Value expectedservicelife = Value(std::string("EXPECTEDSERVICELIFE"));
inline const Value optimisticreferenceservicelife = Value(std::string("OPTIMISTICREFERENCESERVICELIFE"));
inline const Value pessimisticreferenceservicelife = Value(std::string("PESSIMISTICREFERENCESERVICELIFE"));
inline const Value referenceservicelife = Value(std::string("REFERENCESERVICELIFE"));
inline const Value floor = Value(std::string("FLOOR"));
inline const Value roof = Value(std::string("ROOF"));
inline const Value landing = Value(std::string("LANDING"));
inline const Value baseslab = Value(std::string("BASESLAB"));
inline const Value dba = Value(std::string("DBA"));
inline const Value dbb = Value(std::string("DBB"));
inline const Value dbc = Value(std::string("DBC"));
inline const Value nc = Value(std::string("NC"));
inline const Value nr = Value(std::string("NR"));
inline const Value sectionalradiator = Value(std::string("SECTIONALRADIATOR"));
inline const Value panelradiator = Value(std::string("PANELRADIATOR"));
inline const Value tubularradiator = Value(std::string("TUBULARRADIATOR"));
inline const Value convector = Value(std::string("CONVECTOR"));
inline const Value baseboardheater = Value(std::string("BASEBOARDHEATER"));
inline const Value finnedtubeunit = Value(std::string("FINNEDTUBEUNIT"));
inline const Value unitheater = Value(std::string("UNITHEATER"));
inline const Value birdcage = Value(std::string("BIRDCAGE"));
inline const Value cowl = Value(std::string("COWL"));
inline const Value rainwaterhopper = Value(std::string("RAINWATERHOPPER"));
inline const Value winder = Value(std::string("WINDER"));
inline const Value curved = Value(std::string("CURVED"));
inline const Value straight_run_stair = Value(std::string("STRAIGHT_RUN_STAIR"));
inline const Value two_straight_run_stair = Value(std::string("TWO_STRAIGHT_RUN_STAIR"));
inline const Value quarter_winding_stair = Value(std::string("QUARTER_WINDING_STAIR"));
inline const Value quarter_turn_stair = Value(std::string("QUARTER_TURN_STAIR"));
inline const Value half_winding_stair = Value(std::string("HALF_WINDING_STAIR"));
inline const Value half_turn_stair = Value(std::string("HALF_TURN_STAIR"));
inline const Value two_quarter_winding_stair = Value(std::string("TWO_QUARTER_WINDING_STAIR"));
inline const Value two_quarter_turn_stair = Value(std::string("TWO_QUARTER_TURN_STAIR"));
inline const Value three_quarter_winding_stair = Value(std::string("THREE_QUARTER_WINDING_STAIR"));
inline const Value three_quarter_turn_stair = Value(std::string("THREE_QUARTER_TURN_STAIR"));
inline const Value spiral_stair = Value(std::string("SPIRAL_STAIR"));
inline const Value double_return_stair = Value(std::string("DOUBLE_RETURN_STAIR"));
inline const Value curved_run_stair = Value(std::string("CURVED_RUN_STAIR"));
inline const Value two_curved_run_stair = Value(std::string("TWO_CURVED_RUN_STAIR"));
inline const Value readwrite = Value(std::string("READWRITE"));
inline const Value readonly = Value(std::string("READONLY"));
inline const Value locked = Value(std::string("LOCKED"));
inline const Value readwritelocked = Value(std::string("READWRITELOCKED"));
inline const Value readonlylocked = Value(std::string("READONLYLOCKED"));
inline const Value rigid_joined_member = Value(std::string("RIGID_JOINED_MEMBER"));
inline const Value pin_joined_member = Value(std::string("PIN_JOINED_MEMBER"));
inline const Value cable = Value(std::string("CABLE"));
inline const Value tension_member = Value(std::string("TENSION_MEMBER"));
inline const Value compression_member = Value(std::string("COMPRESSION_MEMBER"));
inline const Value bending_element = Value(std::string("BENDING_ELEMENT"));
inline const Value membrane_element = Value(std::string("MEMBRANE_ELEMENT"));
inline const Value shell = Value(std::string("SHELL"));
inline const Value both = Value(std::string("BOTH"));
inline const Value bump = Value(std::string("BUMP"));
inline const Value opacity = Value(std::string("OPACITY"));
inline const Value reflection = Value(std::string("REFLECTION"));
inline const Value selfillumination = Value(std::string("SELFILLUMINATION"));
inline const Value shininess = Value(std::string("SHININESS"));
inline const Value specular = Value(std::string("SPECULAR"));
inline const Value texture = Value(std::string("TEXTURE"));
inline const Value transparencymap = Value(std::string("TRANSPARENCYMAP"));
inline const Value contactor = Value(std::string("CONTACTOR"));
inline const Value emergencystop = Value(std::string("EMERGENCYSTOP"));
inline const Value starter = Value(std::string("STARTER"));
inline const Value switchdisconnector = Value(std::string("SWITCHDISCONNECTOR"));
inline const Value toggleswitch = Value(std::string("TOGGLESWITCH"));
inline const Value preformed = Value(std::string("PREFORMED"));
inline const Value sectional = Value(std::string("SECTIONAL"));
inline const Value expansion = Value(std::string("EXPANSION"));
inline const Value pressurevessel = Value(std::string("PRESSUREVESSEL"));
inline const Value strand = Value(std::string("STRAND"));
inline const Value wire = Value(std::string("WIRE"));
inline const Value bar = Value(std::string("BAR"));
inline const Value coated = Value(std::string("COATED"));
inline const Value up = Value(std::string("UP"));
inline const Value down = Value(std::string("DOWN"));
inline const Value people = Value(std::string("PEOPLE"));
inline const Value lighting = Value(std::string("LIGHTING"));
inline const Value equipment = Value(std::string("EQUIPMENT"));
inline const Value ventilationindoorair = Value(std::string("VENTILATIONINDOORAIR"));
inline const Value ventilationoutsideair = Value(std::string("VENTILATIONOUTSIDEAIR"));
inline const Value recirculatedair = Value(std::string("RECIRCULATEDAIR"));
inline const Value exhaustair = Value(std::string("EXHAUSTAIR"));
inline const Value airexchangerate = Value(std::string("AIREXCHANGERATE"));
inline const Value drybulbtemperature = Value(std::string("DRYBULBTEMPERATURE"));
inline const Value relativehumidity = Value(std::string("RELATIVEHUMIDITY"));
inline const Value infiltration = Value(std::string("INFILTRATION"));
inline const Value sensible = Value(std::string("SENSIBLE"));
inline const Value latent = Value(std::string("LATENT"));
inline const Value radiant = Value(std::string("RADIANT"));
inline const Value continuous = Value(std::string("CONTINUOUS"));
inline const Value discrete = Value(std::string("DISCRETE"));
inline const Value discretebinary = Value(std::string("DISCRETEBINARY"));
inline const Value piecewisebinary = Value(std::string("PIECEWISEBINARY"));
inline const Value piecewiseconstant = Value(std::string("PIECEWISECONSTANT"));
inline const Value piecewisecontinuous = Value(std::string("PIECEWISECONTINUOUS"));
inline const Value annual = Value(std::string("ANNUAL"));
inline const Value monthly = Value(std::string("MONTHLY"));
inline const Value weekly = Value(std::string("WEEKLY"));
inline const Value daily = Value(std::string("DAILY"));
inline const Value frequency = Value(std::string("FREQUENCY"));
inline const Value voltage = Value(std::string("VOLTAGE"));
inline const Value discontinuous = Value(std::string("DISCONTINUOUS"));
inline const Value contsamegradient = Value(std::string("CONTSAMEGRADIENT"));
inline const Value contsamegradientsamecurvature = Value(std::string("CONTSAMEGRADIENTSAMECURVATURE"));
inline const Value elevator = Value(std::string("ELEVATOR"));
inline const Value escalator = Value(std::string("ESCALATOR"));
inline const Value movingwalkway = Value(std::string("MOVINGWALKWAY"));
inline const Value cartesian = Value(std::string("CARTESIAN"));
inline const Value parameter = Value(std::string("PARAMETER"));
inline const Value finned = Value(std::string("FINNED"));
inline const Value absorbeddoseunit = Value(std::string("ABSORBEDDOSEUNIT"));
inline const Value amountofsubstanceunit = Value(std::string("AMOUNTOFSUBSTANCEUNIT"));
inline const Value areaunit = Value(std::string("AREAUNIT"));
inline const Value doseequivalentunit = Value(std::string("DOSEEQUIVALENTUNIT"));
inline const Value electriccapacitanceunit = Value(std::string("ELECTRICCAPACITANCEUNIT"));
inline const Value electricchargeunit = Value(std::string("ELECTRICCHARGEUNIT"));
inline const Value electricconductanceunit = Value(std::string("ELECTRICCONDUCTANCEUNIT"));
inline const Value electriccurrentunit = Value(std::string("ELECTRICCURRENTUNIT"));
inline const Value electricresistanceunit = Value(std::string("ELECTRICRESISTANCEUNIT"));
inline const Value electricvoltageunit = Value(std::string("ELECTRICVOLTAGEUNIT"));
inline const Value energyunit = Value(std::string("ENERGYUNIT"));
inline const Value forceunit = Value(std::string("FORCEUNIT"));
inline const Value frequencyunit = Value(std::string("FREQUENCYUNIT"));
inline const Value illuminanceunit = Value(std::string("ILLUMINANCEUNIT"));
inline const Value inductanceunit = Value(std::string("INDUCTANCEUNIT"));
inline const Value lengthunit = Value(std::string("LENGTHUNIT"));
inline const Value luminousfluxunit = Value(std::string("LUMINOUSFLUXUNIT"));
inline const Value luminousintensityunit = Value(std::string("LUMINOUSINTENSITYUNIT"));
inline const Value magneticfluxdensityunit = Value(std::string("MAGNETICFLUXDENSITYUNIT"));
inline const Value magneticfluxunit = Value(std::string("MAGNETICFLUXUNIT"));
inline const Value massunit = Value(std::string("MASSUNIT"));
inline const Value planeangleunit = Value(std::string("PLANEANGLEUNIT"));
inline const Value powerunit = Value(std::string("POWERUNIT"));
inline const Value pressureunit = Value(std::string("PRESSUREUNIT"));
inline const Value radioactivityunit = Value(std::string("RADIOACTIVITYUNIT"));
inline const Value solidangleunit = Value(std::string("SOLIDANGLEUNIT"));
inline const Value thermodynamictemperatureunit = Value(std::string("THERMODYNAMICTEMPERATUREUNIT"));
inline const Value timeunit = Value(std::string("TIMEUNIT"));
inline const Value volumeunit = Value(std::string("VOLUMEUNIT"));
inline const Value airhandler = Value(std::string("AIRHANDLER"));
inline const Value airconditioningunit = Value(std::string("AIRCONDITIONINGUNIT"));
inline const Value splitsystem = Value(std::string("SPLITSYSTEM"));
inline const Value rooftopunit = Value(std::string("ROOFTOPUNIT"));
inline const Value airrelease = Value(std::string("AIRRELEASE"));
inline const Value antivacuum = Value(std::string("ANTIVACUUM"));
inline const Value changeover = Value(std::string("CHANGEOVER"));
inline const Value check = Value(std::string("CHECK"));
inline const Value diverting = Value(std::string("DIVERTING"));
inline const Value drawoffcock = Value(std::string("DRAWOFFCOCK"));
inline const Value doublecheck = Value(std::string("DOUBLECHECK"));
inline const Value doubleregulating = Value(std::string("DOUBLEREGULATING"));
inline const Value faucet = Value(std::string("FAUCET"));
inline const Value flushing = Value(std::string("FLUSHING"));
inline const Value gascock = Value(std::string("GASCOCK"));
inline const Value gastap = Value(std::string("GASTAP"));
inline const Value isolating = Value(std::string("ISOLATING"));
inline const Value mixing = Value(std::string("MIXING"));
inline const Value pressurereducing = Value(std::string("PRESSUREREDUCING"));
inline const Value pressurerelief = Value(std::string("PRESSURERELIEF"));
inline const Value regulating = Value(std::string("REGULATING"));
inline const Value safetycutoff = Value(std::string("SAFETYCUTOFF"));
inline const Value steamtrap = Value(std::string("STEAMTRAP"));
inline const Value stopcock = Value(std::string("STOPCOCK"));
inline const Value compression = Value(std::string("COMPRESSION"));
inline const Value spring = Value(std::string("SPRING"));
inline const Value standard = Value(std::string("STANDARD"));
inline const Value polygonal = Value(std::string("POLYGONAL"));
inline const Value elementedwall = Value(std::string("ELEMENTEDWALL"));
inline const Value plumbingwall = Value(std::string("PLUMBINGWALL"));
inline const Value floortrap = Value(std::string("FLOORTRAP"));
inline const Value floorwaste = Value(std::string("FLOORWASTE"));
inline const Value gullysump = Value(std::string("GULLYSUMP"));
inline const Value gullytrap = Value(std::string("GULLYTRAP"));
inline const Value greaseinterceptor = Value(std::string("GREASEINTERCEPTOR"));
inline const Value oilinterceptor = Value(std::string("OILINTERCEPTOR"));
inline const Value petrolinterceptor = Value(std::string("PETROLINTERCEPTOR"));
inline const Value roofdrain = Value(std::string("ROOFDRAIN"));
inline const Value wastedisposalunit = Value(std::string("WASTEDISPOSALUNIT"));
inline const Value wastetrap = Value(std::string("WASTETRAP"));
inline const Value sidehungrighthand = Value(std::string("SIDEHUNGRIGHTHAND"));
inline const Value sidehunglefthand = Value(std::string("SIDEHUNGLEFTHAND"));
inline const Value tiltandturnrighthand = Value(std::string("TILTANDTURNRIGHTHAND"));
inline const Value tiltandturnlefthand = Value(std::string("TILTANDTURNLEFTHAND"));
inline const Value tophung = Value(std::string("TOPHUNG"));
inline const Value bottomhung = Value(std::string("BOTTOMHUNG"));
inline const Value pivothorizontal = Value(std::string("PIVOTHORIZONTAL"));
inline const Value pivotvertical = Value(std::string("PIVOTVERTICAL"));
inline const Value slidinghorizontal = Value(std::string("SLIDINGHORIZONTAL"));
inline const Value slidingvertical = Value(std::string("SLIDINGVERTICAL"));
inline const Value removablecasement = Value(std::string("REMOVABLECASEMENT"));
inline const Value fixedcasement = Value(std::string("FIXEDCASEMENT"));
inline const Value otheroperation = Value(std::string("OTHEROPERATION"));
inline const Value bottom = Value(std::string("BOTTOM"));
inline const Value top = Value(std::string("TOP"));
inline const Value other_construction = Value(std::string("OTHER_CONSTRUCTION"));
inline const Value single_panel = Value(std::string("SINGLE_PANEL"));
inline const Value double_panel_vertical = Value(std::string("DOUBLE_PANEL_VERTICAL"));
inline const Value double_panel_horizontal = Value(std::string("DOUBLE_PANEL_HORIZONTAL"));
inline const Value triple_panel_vertical = Value(std::string("TRIPLE_PANEL_VERTICAL"));
inline const Value triple_panel_bottom = Value(std::string("TRIPLE_PANEL_BOTTOM"));
inline const Value triple_panel_top = Value(std::string("TRIPLE_PANEL_TOP"));
inline const Value triple_panel_left = Value(std::string("TRIPLE_PANEL_LEFT"));
inline const Value triple_panel_right = Value(std::string("TRIPLE_PANEL_RIGHT"));
inline const Value triple_panel_horizontal = Value(std::string("TRIPLE_PANEL_HORIZONTAL"));
inline const Value actual = Value(std::string("ACTUAL"));
inline const Value baseline = Value(std::string("BASELINE"));
inline const Value planned = Value(std::string("PLANNED"));

// --- entity constructors ---
inline Value Ifc2DCompositeCurve(Value Segments, Value SelfIntersect) { return ifcapi::express::make_entity("IFC2X3", "Ifc2DCompositeCurve", {{"Segments", Segments}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcActionRequest(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value RequestID) { return ifcapi::express::make_entity("IFC2X3", "IfcActionRequest", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"RequestID", RequestID}}); }
inline Value IfcActor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TheActor) { return ifcapi::express::make_entity("IFC2X3", "IfcActor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TheActor", TheActor}}); }
inline Value IfcActorRole(Value Role, Value UserDefinedRole, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcActorRole", {{"Role", Role}, {"UserDefinedRole", UserDefinedRole}, {"Description", Description}}); }
inline Value IfcActuatorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcActuatorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAddress(Value Purpose, Value Description, Value UserDefinedPurpose) { return ifcapi::express::make_entity("IFC2X3", "IfcAddress", {{"Purpose", Purpose}, {"Description", Description}, {"UserDefinedPurpose", UserDefinedPurpose}}); }
inline Value IfcAirTerminalBoxType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcAirTerminalBoxType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAirTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcAirTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAirToAirHeatRecoveryType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcAirToAirHeatRecoveryType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAlarmType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcAlarmType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcAngularDimension(Value Contents) { return ifcapi::express::make_entity("IFC2X3", "IfcAngularDimension", {{"Contents", Contents}}); }
inline Value IfcAnnotation(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotation", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcAnnotationCurveOccurrence(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationCurveOccurrence", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcAnnotationFillArea(Value OuterBoundary, Value InnerBoundaries) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationFillArea", {{"OuterBoundary", OuterBoundary}, {"InnerBoundaries", InnerBoundaries}}); }
inline Value IfcAnnotationFillAreaOccurrence(Value Item, Value Styles, Value Name, Value FillStyleTarget, Value GlobalOrLocal) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationFillAreaOccurrence", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}, {"FillStyleTarget", FillStyleTarget}, {"GlobalOrLocal", GlobalOrLocal}}); }
inline Value IfcAnnotationOccurrence(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationOccurrence", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcAnnotationSurface(Value Item, Value TextureCoordinates) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationSurface", {{"Item", Item}, {"TextureCoordinates", TextureCoordinates}}); }
inline Value IfcAnnotationSurfaceOccurrence(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationSurfaceOccurrence", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcAnnotationSymbolOccurrence(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationSymbolOccurrence", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcAnnotationTextOccurrence(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcAnnotationTextOccurrence", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcApplication(Value ApplicationDeveloper, Value Version, Value ApplicationFullName, Value ApplicationIdentifier) { return ifcapi::express::make_entity("IFC2X3", "IfcApplication", {{"ApplicationDeveloper", ApplicationDeveloper}, {"Version", Version}, {"ApplicationFullName", ApplicationFullName}, {"ApplicationIdentifier", ApplicationIdentifier}}); }
inline Value IfcAppliedValue(Value Name, Value Description, Value AppliedValue, Value UnitBasis, Value ApplicableDate, Value FixedUntilDate) { return ifcapi::express::make_entity("IFC2X3", "IfcAppliedValue", {{"Name", Name}, {"Description", Description}, {"AppliedValue", AppliedValue}, {"UnitBasis", UnitBasis}, {"ApplicableDate", ApplicableDate}, {"FixedUntilDate", FixedUntilDate}}); }
inline Value IfcAppliedValueRelationship(Value ComponentOfTotal, Value Components, Value ArithmeticOperator, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcAppliedValueRelationship", {{"ComponentOfTotal", ComponentOfTotal}, {"Components", Components}, {"ArithmeticOperator", ArithmeticOperator}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcApproval(Value Description, Value ApprovalDateTime, Value ApprovalStatus, Value ApprovalLevel, Value ApprovalQualifier, Value Name, Value Identifier) { return ifcapi::express::make_entity("IFC2X3", "IfcApproval", {{"Description", Description}, {"ApprovalDateTime", ApprovalDateTime}, {"ApprovalStatus", ApprovalStatus}, {"ApprovalLevel", ApprovalLevel}, {"ApprovalQualifier", ApprovalQualifier}, {"Name", Name}, {"Identifier", Identifier}}); }
inline Value IfcApprovalActorRelationship(Value Actor, Value Approval, Value Role) { return ifcapi::express::make_entity("IFC2X3", "IfcApprovalActorRelationship", {{"Actor", Actor}, {"Approval", Approval}, {"Role", Role}}); }
inline Value IfcApprovalPropertyRelationship(Value ApprovedProperties, Value Approval) { return ifcapi::express::make_entity("IFC2X3", "IfcApprovalPropertyRelationship", {{"ApprovedProperties", ApprovedProperties}, {"Approval", Approval}}); }
inline Value IfcApprovalRelationship(Value RelatedApproval, Value RelatingApproval, Value Description, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcApprovalRelationship", {{"RelatedApproval", RelatedApproval}, {"RelatingApproval", RelatingApproval}, {"Description", Description}, {"Name", Name}}); }
inline Value IfcArbitraryClosedProfileDef(Value ProfileType, Value ProfileName, Value OuterCurve) { return ifcapi::express::make_entity("IFC2X3", "IfcArbitraryClosedProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"OuterCurve", OuterCurve}}); }
inline Value IfcArbitraryOpenProfileDef(Value ProfileType, Value ProfileName, Value Curve) { return ifcapi::express::make_entity("IFC2X3", "IfcArbitraryOpenProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Curve", Curve}}); }
inline Value IfcArbitraryProfileDefWithVoids(Value ProfileType, Value ProfileName, Value OuterCurve, Value InnerCurves) { return ifcapi::express::make_entity("IFC2X3", "IfcArbitraryProfileDefWithVoids", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"OuterCurve", OuterCurve}, {"InnerCurves", InnerCurves}}); }
inline Value IfcAsset(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value AssetID, Value OriginalValue, Value CurrentValue, Value TotalReplacementCost, Value Owner, Value User, Value ResponsiblePerson, Value IncorporationDate, Value DepreciatedValue) { return ifcapi::express::make_entity("IFC2X3", "IfcAsset", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"AssetID", AssetID}, {"OriginalValue", OriginalValue}, {"CurrentValue", CurrentValue}, {"TotalReplacementCost", TotalReplacementCost}, {"Owner", Owner}, {"User", User}, {"ResponsiblePerson", ResponsiblePerson}, {"IncorporationDate", IncorporationDate}, {"DepreciatedValue", DepreciatedValue}}); }
inline Value IfcAsymmetricIShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value OverallWidth, Value OverallDepth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value TopFlangeWidth, Value TopFlangeThickness, Value TopFlangeFilletRadius, Value CentreOfGravityInY) { return ifcapi::express::make_entity("IFC2X3", "IfcAsymmetricIShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"OverallWidth", OverallWidth}, {"OverallDepth", OverallDepth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"TopFlangeWidth", TopFlangeWidth}, {"TopFlangeThickness", TopFlangeThickness}, {"TopFlangeFilletRadius", TopFlangeFilletRadius}, {"CentreOfGravityInY", CentreOfGravityInY}}); }
inline Value IfcAxis1Placement(Value Location, Value Axis) { return ifcapi::express::make_entity("IFC2X3", "IfcAxis1Placement", {{"Location", Location}, {"Axis", Axis}}); }
inline Value IfcAxis2Placement2D(Value Location, Value RefDirection) { return ifcapi::express::make_entity("IFC2X3", "IfcAxis2Placement2D", {{"Location", Location}, {"RefDirection", RefDirection}}); }
inline Value IfcAxis2Placement3D(Value Location, Value Axis, Value RefDirection) { return ifcapi::express::make_entity("IFC2X3", "IfcAxis2Placement3D", {{"Location", Location}, {"Axis", Axis}, {"RefDirection", RefDirection}}); }
inline Value IfcBSplineCurve(Value Degree, Value ControlPointsList, Value CurveForm, Value ClosedCurve, Value SelfIntersect) { return ifcapi::express::make_entity("IFC2X3", "IfcBSplineCurve", {{"Degree", Degree}, {"ControlPointsList", ControlPointsList}, {"CurveForm", CurveForm}, {"ClosedCurve", ClosedCurve}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcBeam(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcBeam", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcBeamType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcBeamType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBezierCurve(Value Degree, Value ControlPointsList, Value CurveForm, Value ClosedCurve, Value SelfIntersect) { return ifcapi::express::make_entity("IFC2X3", "IfcBezierCurve", {{"Degree", Degree}, {"ControlPointsList", ControlPointsList}, {"CurveForm", CurveForm}, {"ClosedCurve", ClosedCurve}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcBlobTexture(Value RepeatS, Value RepeatT, Value TextureType, Value TextureTransform, Value RasterFormat, Value RasterCode) { return ifcapi::express::make_entity("IFC2X3", "IfcBlobTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"TextureType", TextureType}, {"TextureTransform", TextureTransform}, {"RasterFormat", RasterFormat}, {"RasterCode", RasterCode}}); }
inline Value IfcBlock(Value Position, Value XLength, Value YLength, Value ZLength) { return ifcapi::express::make_entity("IFC2X3", "IfcBlock", {{"Position", Position}, {"XLength", XLength}, {"YLength", YLength}, {"ZLength", ZLength}}); }
inline Value IfcBoilerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcBoilerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBooleanClippingResult(Value Operator_, Value FirstOperand, Value SecondOperand) { return ifcapi::express::make_entity("IFC2X3", "IfcBooleanClippingResult", {{"Operator", Operator_}, {"FirstOperand", FirstOperand}, {"SecondOperand", SecondOperand}}); }
inline Value IfcBooleanResult(Value Operator_, Value FirstOperand, Value SecondOperand) { return ifcapi::express::make_entity("IFC2X3", "IfcBooleanResult", {{"Operator", Operator_}, {"FirstOperand", FirstOperand}, {"SecondOperand", SecondOperand}}); }
inline Value IfcBoundaryCondition(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcBoundaryCondition", {{"Name", Name}}); }
inline Value IfcBoundaryEdgeCondition(Value Name, Value LinearStiffnessByLengthX, Value LinearStiffnessByLengthY, Value LinearStiffnessByLengthZ, Value RotationalStiffnessByLengthX, Value RotationalStiffnessByLengthY, Value RotationalStiffnessByLengthZ) { return ifcapi::express::make_entity("IFC2X3", "IfcBoundaryEdgeCondition", {{"Name", Name}, {"LinearStiffnessByLengthX", LinearStiffnessByLengthX}, {"LinearStiffnessByLengthY", LinearStiffnessByLengthY}, {"LinearStiffnessByLengthZ", LinearStiffnessByLengthZ}, {"RotationalStiffnessByLengthX", RotationalStiffnessByLengthX}, {"RotationalStiffnessByLengthY", RotationalStiffnessByLengthY}, {"RotationalStiffnessByLengthZ", RotationalStiffnessByLengthZ}}); }
inline Value IfcBoundaryFaceCondition(Value Name, Value LinearStiffnessByAreaX, Value LinearStiffnessByAreaY, Value LinearStiffnessByAreaZ) { return ifcapi::express::make_entity("IFC2X3", "IfcBoundaryFaceCondition", {{"Name", Name}, {"LinearStiffnessByAreaX", LinearStiffnessByAreaX}, {"LinearStiffnessByAreaY", LinearStiffnessByAreaY}, {"LinearStiffnessByAreaZ", LinearStiffnessByAreaZ}}); }
inline Value IfcBoundaryNodeCondition(Value Name, Value LinearStiffnessX, Value LinearStiffnessY, Value LinearStiffnessZ, Value RotationalStiffnessX, Value RotationalStiffnessY, Value RotationalStiffnessZ) { return ifcapi::express::make_entity("IFC2X3", "IfcBoundaryNodeCondition", {{"Name", Name}, {"LinearStiffnessX", LinearStiffnessX}, {"LinearStiffnessY", LinearStiffnessY}, {"LinearStiffnessZ", LinearStiffnessZ}, {"RotationalStiffnessX", RotationalStiffnessX}, {"RotationalStiffnessY", RotationalStiffnessY}, {"RotationalStiffnessZ", RotationalStiffnessZ}}); }
inline Value IfcBoundaryNodeConditionWarping(Value Name, Value LinearStiffnessX, Value LinearStiffnessY, Value LinearStiffnessZ, Value RotationalStiffnessX, Value RotationalStiffnessY, Value RotationalStiffnessZ, Value WarpingStiffness) { return ifcapi::express::make_entity("IFC2X3", "IfcBoundaryNodeConditionWarping", {{"Name", Name}, {"LinearStiffnessX", LinearStiffnessX}, {"LinearStiffnessY", LinearStiffnessY}, {"LinearStiffnessZ", LinearStiffnessZ}, {"RotationalStiffnessX", RotationalStiffnessX}, {"RotationalStiffnessY", RotationalStiffnessY}, {"RotationalStiffnessZ", RotationalStiffnessZ}, {"WarpingStiffness", WarpingStiffness}}); }
inline Value IfcBoundedCurve() { return ifcapi::express::make_entity("IFC2X3", "IfcBoundedCurve", {}); }
inline Value IfcBoundedSurface() { return ifcapi::express::make_entity("IFC2X3", "IfcBoundedSurface", {}); }
inline Value IfcBoundingBox(Value Corner, Value XDim, Value YDim, Value ZDim) { return ifcapi::express::make_entity("IFC2X3", "IfcBoundingBox", {{"Corner", Corner}, {"XDim", XDim}, {"YDim", YDim}, {"ZDim", ZDim}}); }
inline Value IfcBoxedHalfSpace(Value BaseSurface, Value AgreementFlag, Value Enclosure) { return ifcapi::express::make_entity("IFC2X3", "IfcBoxedHalfSpace", {{"BaseSurface", BaseSurface}, {"AgreementFlag", AgreementFlag}, {"Enclosure", Enclosure}}); }
inline Value IfcBuilding(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value ElevationOfRefHeight, Value ElevationOfTerrain, Value BuildingAddress) { return ifcapi::express::make_entity("IFC2X3", "IfcBuilding", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"ElevationOfRefHeight", ElevationOfRefHeight}, {"ElevationOfTerrain", ElevationOfTerrain}, {"BuildingAddress", BuildingAddress}}); }
inline Value IfcBuildingElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcBuildingElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcBuildingElementComponent(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcBuildingElementComponent", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcBuildingElementPart(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcBuildingElementPart", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcBuildingElementProxy(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value CompositionType) { return ifcapi::express::make_entity("IFC2X3", "IfcBuildingElementProxy", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"CompositionType", CompositionType}}); }
inline Value IfcBuildingElementProxyType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcBuildingElementProxyType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcBuildingElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcBuildingElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcBuildingStorey(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value Elevation) { return ifcapi::express::make_entity("IFC2X3", "IfcBuildingStorey", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"Elevation", Elevation}}); }
inline Value IfcCShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value Width, Value WallThickness, Value Girth, Value InternalFilletRadius, Value CentreOfGravityInX) { return ifcapi::express::make_entity("IFC2X3", "IfcCShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"Width", Width}, {"WallThickness", WallThickness}, {"Girth", Girth}, {"InternalFilletRadius", InternalFilletRadius}, {"CentreOfGravityInX", CentreOfGravityInX}}); }
inline Value IfcCableCarrierFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCableCarrierFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableCarrierSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCableCarrierSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCableSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCableSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCalendarDate(Value DayComponent, Value MonthComponent, Value YearComponent) { return ifcapi::express::make_entity("IFC2X3", "IfcCalendarDate", {{"DayComponent", DayComponent}, {"MonthComponent", MonthComponent}, {"YearComponent", YearComponent}}); }
inline Value IfcCartesianPoint(Value Coordinates) { return ifcapi::express::make_entity("IFC2X3", "IfcCartesianPoint", {{"Coordinates", Coordinates}}); }
inline Value IfcCartesianTransformationOperator(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale) { return ifcapi::express::make_entity("IFC2X3", "IfcCartesianTransformationOperator", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}}); }
inline Value IfcCartesianTransformationOperator2D(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale) { return ifcapi::express::make_entity("IFC2X3", "IfcCartesianTransformationOperator2D", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}}); }
inline Value IfcCartesianTransformationOperator2DnonUniform(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale, Value Scale2) { return ifcapi::express::make_entity("IFC2X3", "IfcCartesianTransformationOperator2DnonUniform", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}, {"Scale2", Scale2}}); }
inline Value IfcCartesianTransformationOperator3D(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale, Value Axis3) { return ifcapi::express::make_entity("IFC2X3", "IfcCartesianTransformationOperator3D", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}, {"Axis3", Axis3}}); }
inline Value IfcCartesianTransformationOperator3DnonUniform(Value Axis1, Value Axis2, Value LocalOrigin, Value Scale, Value Axis3, Value Scale2, Value Scale3) { return ifcapi::express::make_entity("IFC2X3", "IfcCartesianTransformationOperator3DnonUniform", {{"Axis1", Axis1}, {"Axis2", Axis2}, {"LocalOrigin", LocalOrigin}, {"Scale", Scale}, {"Axis3", Axis3}, {"Scale2", Scale2}, {"Scale3", Scale3}}); }
inline Value IfcCenterLineProfileDef(Value ProfileType, Value ProfileName, Value Curve, Value Thickness) { return ifcapi::express::make_entity("IFC2X3", "IfcCenterLineProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Curve", Curve}, {"Thickness", Thickness}}); }
inline Value IfcChamferEdgeFeature(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value FeatureLength, Value Width, Value Height) { return ifcapi::express::make_entity("IFC2X3", "IfcChamferEdgeFeature", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"FeatureLength", FeatureLength}, {"Width", Width}, {"Height", Height}}); }
inline Value IfcChillerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcChillerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCircle(Value Position, Value Radius) { return ifcapi::express::make_entity("IFC2X3", "IfcCircle", {{"Position", Position}, {"Radius", Radius}}); }
inline Value IfcCircleHollowProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Radius, Value WallThickness) { return ifcapi::express::make_entity("IFC2X3", "IfcCircleHollowProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Radius", Radius}, {"WallThickness", WallThickness}}); }
inline Value IfcCircleProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Radius) { return ifcapi::express::make_entity("IFC2X3", "IfcCircleProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Radius", Radius}}); }
inline Value IfcClassification(Value Source, Value Edition, Value EditionDate, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcClassification", {{"Source", Source}, {"Edition", Edition}, {"EditionDate", EditionDate}, {"Name", Name}}); }
inline Value IfcClassificationItem(Value Notation, Value ItemOf, Value Title) { return ifcapi::express::make_entity("IFC2X3", "IfcClassificationItem", {{"Notation", Notation}, {"ItemOf", ItemOf}, {"Title", Title}}); }
inline Value IfcClassificationItemRelationship(Value RelatingItem, Value RelatedItems) { return ifcapi::express::make_entity("IFC2X3", "IfcClassificationItemRelationship", {{"RelatingItem", RelatingItem}, {"RelatedItems", RelatedItems}}); }
inline Value IfcClassificationNotation(Value NotationFacets) { return ifcapi::express::make_entity("IFC2X3", "IfcClassificationNotation", {{"NotationFacets", NotationFacets}}); }
inline Value IfcClassificationNotationFacet(Value NotationValue) { return ifcapi::express::make_entity("IFC2X3", "IfcClassificationNotationFacet", {{"NotationValue", NotationValue}}); }
inline Value IfcClassificationReference(Value Location, Value ItemReference, Value Name, Value ReferencedSource) { return ifcapi::express::make_entity("IFC2X3", "IfcClassificationReference", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}, {"ReferencedSource", ReferencedSource}}); }
inline Value IfcClosedShell(Value CfsFaces) { return ifcapi::express::make_entity("IFC2X3", "IfcClosedShell", {{"CfsFaces", CfsFaces}}); }
inline Value IfcCoilType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCoilType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcColourRgb(Value Name, Value Red, Value Green, Value Blue) { return ifcapi::express::make_entity("IFC2X3", "IfcColourRgb", {{"Name", Name}, {"Red", Red}, {"Green", Green}, {"Blue", Blue}}); }
inline Value IfcColourSpecification(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcColourSpecification", {{"Name", Name}}); }
inline Value IfcColumn(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcColumn", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcColumnType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcColumnType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcComplexProperty(Value Name, Value Description, Value UsageName, Value HasProperties) { return ifcapi::express::make_entity("IFC2X3", "IfcComplexProperty", {{"Name", Name}, {"Description", Description}, {"UsageName", UsageName}, {"HasProperties", HasProperties}}); }
inline Value IfcCompositeCurve(Value Segments, Value SelfIntersect) { return ifcapi::express::make_entity("IFC2X3", "IfcCompositeCurve", {{"Segments", Segments}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcCompositeCurveSegment(Value Transition, Value SameSense, Value ParentCurve) { return ifcapi::express::make_entity("IFC2X3", "IfcCompositeCurveSegment", {{"Transition", Transition}, {"SameSense", SameSense}, {"ParentCurve", ParentCurve}}); }
inline Value IfcCompositeProfileDef(Value ProfileType, Value ProfileName, Value Profiles, Value Label) { return ifcapi::express::make_entity("IFC2X3", "IfcCompositeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Profiles", Profiles}, {"Label", Label}}); }
inline Value IfcCompressorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCompressorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCondenserType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCondenserType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCondition(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcCondition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcConditionCriterion(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Criterion, Value CriterionDateTime) { return ifcapi::express::make_entity("IFC2X3", "IfcConditionCriterion", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Criterion", Criterion}, {"CriterionDateTime", CriterionDateTime}}); }
inline Value IfcConic(Value Position) { return ifcapi::express::make_entity("IFC2X3", "IfcConic", {{"Position", Position}}); }
inline Value IfcConnectedFaceSet(Value CfsFaces) { return ifcapi::express::make_entity("IFC2X3", "IfcConnectedFaceSet", {{"CfsFaces", CfsFaces}}); }
inline Value IfcConnectionCurveGeometry(Value CurveOnRelatingElement, Value CurveOnRelatedElement) { return ifcapi::express::make_entity("IFC2X3", "IfcConnectionCurveGeometry", {{"CurveOnRelatingElement", CurveOnRelatingElement}, {"CurveOnRelatedElement", CurveOnRelatedElement}}); }
inline Value IfcConnectionGeometry() { return ifcapi::express::make_entity("IFC2X3", "IfcConnectionGeometry", {}); }
inline Value IfcConnectionPointEccentricity(Value PointOnRelatingElement, Value PointOnRelatedElement, Value EccentricityInX, Value EccentricityInY, Value EccentricityInZ) { return ifcapi::express::make_entity("IFC2X3", "IfcConnectionPointEccentricity", {{"PointOnRelatingElement", PointOnRelatingElement}, {"PointOnRelatedElement", PointOnRelatedElement}, {"EccentricityInX", EccentricityInX}, {"EccentricityInY", EccentricityInY}, {"EccentricityInZ", EccentricityInZ}}); }
inline Value IfcConnectionPointGeometry(Value PointOnRelatingElement, Value PointOnRelatedElement) { return ifcapi::express::make_entity("IFC2X3", "IfcConnectionPointGeometry", {{"PointOnRelatingElement", PointOnRelatingElement}, {"PointOnRelatedElement", PointOnRelatedElement}}); }
inline Value IfcConnectionPortGeometry(Value LocationAtRelatingElement, Value LocationAtRelatedElement, Value ProfileOfPort) { return ifcapi::express::make_entity("IFC2X3", "IfcConnectionPortGeometry", {{"LocationAtRelatingElement", LocationAtRelatingElement}, {"LocationAtRelatedElement", LocationAtRelatedElement}, {"ProfileOfPort", ProfileOfPort}}); }
inline Value IfcConnectionSurfaceGeometry(Value SurfaceOnRelatingElement, Value SurfaceOnRelatedElement) { return ifcapi::express::make_entity("IFC2X3", "IfcConnectionSurfaceGeometry", {{"SurfaceOnRelatingElement", SurfaceOnRelatingElement}, {"SurfaceOnRelatedElement", SurfaceOnRelatedElement}}); }
inline Value IfcConstraint(Value Name, Value Description, Value ConstraintGrade, Value ConstraintSource, Value CreatingActor, Value CreationTime, Value UserDefinedGrade) { return ifcapi::express::make_entity("IFC2X3", "IfcConstraint", {{"Name", Name}, {"Description", Description}, {"ConstraintGrade", ConstraintGrade}, {"ConstraintSource", ConstraintSource}, {"CreatingActor", CreatingActor}, {"CreationTime", CreationTime}, {"UserDefinedGrade", UserDefinedGrade}}); }
inline Value IfcConstraintAggregationRelationship(Value Name, Value Description, Value RelatingConstraint, Value RelatedConstraints, Value LogicalAggregator) { return ifcapi::express::make_entity("IFC2X3", "IfcConstraintAggregationRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingConstraint", RelatingConstraint}, {"RelatedConstraints", RelatedConstraints}, {"LogicalAggregator", LogicalAggregator}}); }
inline Value IfcConstraintClassificationRelationship(Value ClassifiedConstraint, Value RelatedClassifications) { return ifcapi::express::make_entity("IFC2X3", "IfcConstraintClassificationRelationship", {{"ClassifiedConstraint", ClassifiedConstraint}, {"RelatedClassifications", RelatedClassifications}}); }
inline Value IfcConstraintRelationship(Value Name, Value Description, Value RelatingConstraint, Value RelatedConstraints) { return ifcapi::express::make_entity("IFC2X3", "IfcConstraintRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingConstraint", RelatingConstraint}, {"RelatedConstraints", RelatedConstraints}}); }
inline Value IfcConstructionEquipmentResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ResourceIdentifier, Value ResourceGroup, Value ResourceConsumption, Value BaseQuantity) { return ifcapi::express::make_entity("IFC2X3", "IfcConstructionEquipmentResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ResourceIdentifier", ResourceIdentifier}, {"ResourceGroup", ResourceGroup}, {"ResourceConsumption", ResourceConsumption}, {"BaseQuantity", BaseQuantity}}); }
inline Value IfcConstructionMaterialResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ResourceIdentifier, Value ResourceGroup, Value ResourceConsumption, Value BaseQuantity, Value Suppliers, Value UsageRatio) { return ifcapi::express::make_entity("IFC2X3", "IfcConstructionMaterialResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ResourceIdentifier", ResourceIdentifier}, {"ResourceGroup", ResourceGroup}, {"ResourceConsumption", ResourceConsumption}, {"BaseQuantity", BaseQuantity}, {"Suppliers", Suppliers}, {"UsageRatio", UsageRatio}}); }
inline Value IfcConstructionProductResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ResourceIdentifier, Value ResourceGroup, Value ResourceConsumption, Value BaseQuantity) { return ifcapi::express::make_entity("IFC2X3", "IfcConstructionProductResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ResourceIdentifier", ResourceIdentifier}, {"ResourceGroup", ResourceGroup}, {"ResourceConsumption", ResourceConsumption}, {"BaseQuantity", BaseQuantity}}); }
inline Value IfcConstructionResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ResourceIdentifier, Value ResourceGroup, Value ResourceConsumption, Value BaseQuantity) { return ifcapi::express::make_entity("IFC2X3", "IfcConstructionResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ResourceIdentifier", ResourceIdentifier}, {"ResourceGroup", ResourceGroup}, {"ResourceConsumption", ResourceConsumption}, {"BaseQuantity", BaseQuantity}}); }
inline Value IfcContextDependentUnit(Value Dimensions, Value UnitType, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcContextDependentUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}, {"Name", Name}}); }
inline Value IfcControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcControllerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcControllerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcConversionBasedUnit(Value Dimensions, Value UnitType, Value Name, Value ConversionFactor) { return ifcapi::express::make_entity("IFC2X3", "IfcConversionBasedUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}, {"Name", Name}, {"ConversionFactor", ConversionFactor}}); }
inline Value IfcCooledBeamType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCooledBeamType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoolingTowerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCoolingTowerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoordinatedUniversalTimeOffset(Value HourOffset, Value MinuteOffset, Value Sense) { return ifcapi::express::make_entity("IFC2X3", "IfcCoordinatedUniversalTimeOffset", {{"HourOffset", HourOffset}, {"MinuteOffset", MinuteOffset}, {"Sense", Sense}}); }
inline Value IfcCostItem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcCostItem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcCostSchedule(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value SubmittedBy, Value PreparedBy, Value SubmittedOn, Value Status, Value TargetUsers, Value UpdateDate, Value ID, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCostSchedule", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"SubmittedBy", SubmittedBy}, {"PreparedBy", PreparedBy}, {"SubmittedOn", SubmittedOn}, {"Status", Status}, {"TargetUsers", TargetUsers}, {"UpdateDate", UpdateDate}, {"ID", ID}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCostValue(Value Name, Value Description, Value AppliedValue, Value UnitBasis, Value ApplicableDate, Value FixedUntilDate, Value CostType, Value Condition) { return ifcapi::express::make_entity("IFC2X3", "IfcCostValue", {{"Name", Name}, {"Description", Description}, {"AppliedValue", AppliedValue}, {"UnitBasis", UnitBasis}, {"ApplicableDate", ApplicableDate}, {"FixedUntilDate", FixedUntilDate}, {"CostType", CostType}, {"Condition", Condition}}); }
inline Value IfcCovering(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCovering", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCoveringType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCoveringType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCraneRailAShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value OverallHeight, Value BaseWidth2, Value Radius, Value HeadWidth, Value HeadDepth2, Value HeadDepth3, Value WebThickness, Value BaseWidth4, Value BaseDepth1, Value BaseDepth2, Value BaseDepth3, Value CentreOfGravityInY) { return ifcapi::express::make_entity("IFC2X3", "IfcCraneRailAShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"OverallHeight", OverallHeight}, {"BaseWidth2", BaseWidth2}, {"Radius", Radius}, {"HeadWidth", HeadWidth}, {"HeadDepth2", HeadDepth2}, {"HeadDepth3", HeadDepth3}, {"WebThickness", WebThickness}, {"BaseWidth4", BaseWidth4}, {"BaseDepth1", BaseDepth1}, {"BaseDepth2", BaseDepth2}, {"BaseDepth3", BaseDepth3}, {"CentreOfGravityInY", CentreOfGravityInY}}); }
inline Value IfcCraneRailFShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value OverallHeight, Value HeadWidth, Value Radius, Value HeadDepth2, Value HeadDepth3, Value WebThickness, Value BaseDepth1, Value BaseDepth2, Value CentreOfGravityInY) { return ifcapi::express::make_entity("IFC2X3", "IfcCraneRailFShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"OverallHeight", OverallHeight}, {"HeadWidth", HeadWidth}, {"Radius", Radius}, {"HeadDepth2", HeadDepth2}, {"HeadDepth3", HeadDepth3}, {"WebThickness", WebThickness}, {"BaseDepth1", BaseDepth1}, {"BaseDepth2", BaseDepth2}, {"CentreOfGravityInY", CentreOfGravityInY}}); }
inline Value IfcCrewResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ResourceIdentifier, Value ResourceGroup, Value ResourceConsumption, Value BaseQuantity) { return ifcapi::express::make_entity("IFC2X3", "IfcCrewResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ResourceIdentifier", ResourceIdentifier}, {"ResourceGroup", ResourceGroup}, {"ResourceConsumption", ResourceConsumption}, {"BaseQuantity", BaseQuantity}}); }
inline Value IfcCsgPrimitive3D(Value Position) { return ifcapi::express::make_entity("IFC2X3", "IfcCsgPrimitive3D", {{"Position", Position}}); }
inline Value IfcCsgSolid(Value TreeRootExpression) { return ifcapi::express::make_entity("IFC2X3", "IfcCsgSolid", {{"TreeRootExpression", TreeRootExpression}}); }
inline Value IfcCurrencyRelationship(Value RelatingMonetaryUnit, Value RelatedMonetaryUnit, Value ExchangeRate, Value RateDateTime, Value RateSource) { return ifcapi::express::make_entity("IFC2X3", "IfcCurrencyRelationship", {{"RelatingMonetaryUnit", RelatingMonetaryUnit}, {"RelatedMonetaryUnit", RelatedMonetaryUnit}, {"ExchangeRate", ExchangeRate}, {"RateDateTime", RateDateTime}, {"RateSource", RateSource}}); }
inline Value IfcCurtainWall(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcCurtainWall", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcCurtainWallType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcCurtainWallType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcCurve() { return ifcapi::express::make_entity("IFC2X3", "IfcCurve", {}); }
inline Value IfcCurveBoundedPlane(Value BasisSurface, Value OuterBoundary, Value InnerBoundaries) { return ifcapi::express::make_entity("IFC2X3", "IfcCurveBoundedPlane", {{"BasisSurface", BasisSurface}, {"OuterBoundary", OuterBoundary}, {"InnerBoundaries", InnerBoundaries}}); }
inline Value IfcCurveStyle(Value Name, Value CurveFont, Value CurveWidth, Value CurveColour) { return ifcapi::express::make_entity("IFC2X3", "IfcCurveStyle", {{"Name", Name}, {"CurveFont", CurveFont}, {"CurveWidth", CurveWidth}, {"CurveColour", CurveColour}}); }
inline Value IfcCurveStyleFont(Value Name, Value PatternList) { return ifcapi::express::make_entity("IFC2X3", "IfcCurveStyleFont", {{"Name", Name}, {"PatternList", PatternList}}); }
inline Value IfcCurveStyleFontAndScaling(Value Name, Value CurveFont, Value CurveFontScaling) { return ifcapi::express::make_entity("IFC2X3", "IfcCurveStyleFontAndScaling", {{"Name", Name}, {"CurveFont", CurveFont}, {"CurveFontScaling", CurveFontScaling}}); }
inline Value IfcCurveStyleFontPattern(Value VisibleSegmentLength, Value InvisibleSegmentLength) { return ifcapi::express::make_entity("IFC2X3", "IfcCurveStyleFontPattern", {{"VisibleSegmentLength", VisibleSegmentLength}, {"InvisibleSegmentLength", InvisibleSegmentLength}}); }
inline Value IfcDamperType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcDamperType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDateAndTime(Value DateComponent, Value TimeComponent) { return ifcapi::express::make_entity("IFC2X3", "IfcDateAndTime", {{"DateComponent", DateComponent}, {"TimeComponent", TimeComponent}}); }
inline Value IfcDefinedSymbol(Value Definition, Value Target) { return ifcapi::express::make_entity("IFC2X3", "IfcDefinedSymbol", {{"Definition", Definition}, {"Target", Target}}); }
inline Value IfcDerivedProfileDef(Value ProfileType, Value ProfileName, Value ParentProfile, Value Operator_, Value Label) { return ifcapi::express::make_entity("IFC2X3", "IfcDerivedProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"ParentProfile", ParentProfile}, {"Operator", Operator_}, {"Label", Label}}); }
inline Value IfcDerivedUnit(Value Elements, Value UnitType, Value UserDefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcDerivedUnit", {{"Elements", Elements}, {"UnitType", UnitType}, {"UserDefinedType", UserDefinedType}}); }
inline Value IfcDerivedUnitElement(Value Unit, Value Exponent) { return ifcapi::express::make_entity("IFC2X3", "IfcDerivedUnitElement", {{"Unit", Unit}, {"Exponent", Exponent}}); }
inline Value IfcDiameterDimension(Value Contents) { return ifcapi::express::make_entity("IFC2X3", "IfcDiameterDimension", {{"Contents", Contents}}); }
inline Value IfcDimensionCalloutRelationship(Value Name, Value Description, Value RelatingDraughtingCallout, Value RelatedDraughtingCallout) { return ifcapi::express::make_entity("IFC2X3", "IfcDimensionCalloutRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingDraughtingCallout", RelatingDraughtingCallout}, {"RelatedDraughtingCallout", RelatedDraughtingCallout}}); }
inline Value IfcDimensionCurve(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcDimensionCurve", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcDimensionCurveDirectedCallout(Value Contents) { return ifcapi::express::make_entity("IFC2X3", "IfcDimensionCurveDirectedCallout", {{"Contents", Contents}}); }
inline Value IfcDimensionCurveTerminator(Value Item, Value Styles, Value Name, Value AnnotatedCurve, Value Role) { return ifcapi::express::make_entity("IFC2X3", "IfcDimensionCurveTerminator", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}, {"AnnotatedCurve", AnnotatedCurve}, {"Role", Role}}); }
inline Value IfcDimensionPair(Value Name, Value Description, Value RelatingDraughtingCallout, Value RelatedDraughtingCallout) { return ifcapi::express::make_entity("IFC2X3", "IfcDimensionPair", {{"Name", Name}, {"Description", Description}, {"RelatingDraughtingCallout", RelatingDraughtingCallout}, {"RelatedDraughtingCallout", RelatedDraughtingCallout}}); }
inline Value IfcDimensionalExponents(Value LengthExponent, Value MassExponent, Value TimeExponent, Value ElectricCurrentExponent, Value ThermodynamicTemperatureExponent, Value AmountOfSubstanceExponent, Value LuminousIntensityExponent) { return ifcapi::express::make_entity("IFC2X3", "IfcDimensionalExponents", {{"LengthExponent", LengthExponent}, {"MassExponent", MassExponent}, {"TimeExponent", TimeExponent}, {"ElectricCurrentExponent", ElectricCurrentExponent}, {"ThermodynamicTemperatureExponent", ThermodynamicTemperatureExponent}, {"AmountOfSubstanceExponent", AmountOfSubstanceExponent}, {"LuminousIntensityExponent", LuminousIntensityExponent}}); }
inline Value IfcDirection(Value DirectionRatios) { return ifcapi::express::make_entity("IFC2X3", "IfcDirection", {{"DirectionRatios", DirectionRatios}}); }
inline Value IfcDiscreteAccessory(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcDiscreteAccessory", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcDiscreteAccessoryType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcDiscreteAccessoryType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcDistributionChamberElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionChamberElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcDistributionChamberElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionChamberElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDistributionControlElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value ControlElementId) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionControlElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"ControlElementId", ControlElementId}}); }
inline Value IfcDistributionControlElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionControlElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcDistributionElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcDistributionElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcDistributionFlowElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionFlowElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcDistributionFlowElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionFlowElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcDistributionPort(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value FlowDirection) { return ifcapi::express::make_entity("IFC2X3", "IfcDistributionPort", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"FlowDirection", FlowDirection}}); }
inline Value IfcDocumentElectronicFormat(Value FileExtension, Value MimeContentType, Value MimeSubtype) { return ifcapi::express::make_entity("IFC2X3", "IfcDocumentElectronicFormat", {{"FileExtension", FileExtension}, {"MimeContentType", MimeContentType}, {"MimeSubtype", MimeSubtype}}); }
inline Value IfcDocumentInformation(Value DocumentId, Value Name, Value Description, Value DocumentReferences, Value Purpose, Value IntendedUse, Value Scope, Value Revision, Value DocumentOwner, Value Editors, Value CreationTime, Value LastRevisionTime, Value ElectronicFormat, Value ValidFrom, Value ValidUntil, Value Confidentiality, Value Status) { return ifcapi::express::make_entity("IFC2X3", "IfcDocumentInformation", {{"DocumentId", DocumentId}, {"Name", Name}, {"Description", Description}, {"DocumentReferences", DocumentReferences}, {"Purpose", Purpose}, {"IntendedUse", IntendedUse}, {"Scope", Scope}, {"Revision", Revision}, {"DocumentOwner", DocumentOwner}, {"Editors", Editors}, {"CreationTime", CreationTime}, {"LastRevisionTime", LastRevisionTime}, {"ElectronicFormat", ElectronicFormat}, {"ValidFrom", ValidFrom}, {"ValidUntil", ValidUntil}, {"Confidentiality", Confidentiality}, {"Status", Status}}); }
inline Value IfcDocumentInformationRelationship(Value RelatingDocument, Value RelatedDocuments, Value RelationshipType) { return ifcapi::express::make_entity("IFC2X3", "IfcDocumentInformationRelationship", {{"RelatingDocument", RelatingDocument}, {"RelatedDocuments", RelatedDocuments}, {"RelationshipType", RelationshipType}}); }
inline Value IfcDocumentReference(Value Location, Value ItemReference, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcDocumentReference", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}}); }
inline Value IfcDoor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value OverallHeight, Value OverallWidth) { return ifcapi::express::make_entity("IFC2X3", "IfcDoor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"OverallHeight", OverallHeight}, {"OverallWidth", OverallWidth}}); }
inline Value IfcDoorLiningProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value LiningDepth, Value LiningThickness, Value ThresholdDepth, Value ThresholdThickness, Value TransomThickness, Value TransomOffset, Value LiningOffset, Value ThresholdOffset, Value CasingThickness, Value CasingDepth, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC2X3", "IfcDoorLiningProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"LiningDepth", LiningDepth}, {"LiningThickness", LiningThickness}, {"ThresholdDepth", ThresholdDepth}, {"ThresholdThickness", ThresholdThickness}, {"TransomThickness", TransomThickness}, {"TransomOffset", TransomOffset}, {"LiningOffset", LiningOffset}, {"ThresholdOffset", ThresholdOffset}, {"CasingThickness", CasingThickness}, {"CasingDepth", CasingDepth}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcDoorPanelProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value PanelDepth, Value PanelOperation, Value PanelWidth, Value PanelPosition, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC2X3", "IfcDoorPanelProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"PanelDepth", PanelDepth}, {"PanelOperation", PanelOperation}, {"PanelWidth", PanelWidth}, {"PanelPosition", PanelPosition}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcDoorStyle(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value OperationType, Value ConstructionType, Value ParameterTakesPrecedence, Value Sizeable) { return ifcapi::express::make_entity("IFC2X3", "IfcDoorStyle", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"OperationType", OperationType}, {"ConstructionType", ConstructionType}, {"ParameterTakesPrecedence", ParameterTakesPrecedence}, {"Sizeable", Sizeable}}); }
inline Value IfcDraughtingCallout(Value Contents) { return ifcapi::express::make_entity("IFC2X3", "IfcDraughtingCallout", {{"Contents", Contents}}); }
inline Value IfcDraughtingCalloutRelationship(Value Name, Value Description, Value RelatingDraughtingCallout, Value RelatedDraughtingCallout) { return ifcapi::express::make_entity("IFC2X3", "IfcDraughtingCalloutRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingDraughtingCallout", RelatingDraughtingCallout}, {"RelatedDraughtingCallout", RelatedDraughtingCallout}}); }
inline Value IfcDraughtingPreDefinedColour(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcDraughtingPreDefinedColour", {{"Name", Name}}); }
inline Value IfcDraughtingPreDefinedCurveFont(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcDraughtingPreDefinedCurveFont", {{"Name", Name}}); }
inline Value IfcDraughtingPreDefinedTextFont(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcDraughtingPreDefinedTextFont", {{"Name", Name}}); }
inline Value IfcDuctFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcDuctFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDuctSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcDuctSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcDuctSilencerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcDuctSilencerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEdge(Value EdgeStart, Value EdgeEnd) { return ifcapi::express::make_entity("IFC2X3", "IfcEdge", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}}); }
inline Value IfcEdgeCurve(Value EdgeStart, Value EdgeEnd, Value EdgeGeometry, Value SameSense) { return ifcapi::express::make_entity("IFC2X3", "IfcEdgeCurve", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}, {"EdgeGeometry", EdgeGeometry}, {"SameSense", SameSense}}); }
inline Value IfcEdgeFeature(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value FeatureLength) { return ifcapi::express::make_entity("IFC2X3", "IfcEdgeFeature", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"FeatureLength", FeatureLength}}); }
inline Value IfcEdgeLoop(Value EdgeList) { return ifcapi::express::make_entity("IFC2X3", "IfcEdgeLoop", {{"EdgeList", EdgeList}}); }
inline Value IfcElectricApplianceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricApplianceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricDistributionPoint(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value DistributionPointFunction, Value UserDefinedFunction) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricDistributionPoint", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"DistributionPointFunction", DistributionPointFunction}, {"UserDefinedFunction", UserDefinedFunction}}); }
inline Value IfcElectricFlowStorageDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricFlowStorageDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricGeneratorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricGeneratorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricHeaterType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricHeaterType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricMotorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricMotorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricTimeControlType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricTimeControlType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElectricalBaseProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value EnergySequence, Value UserDefinedEnergySequence, Value ElectricCurrentType, Value InputVoltage, Value InputFrequency, Value FullLoadCurrent, Value MinimumCircuitCurrent, Value MaximumPowerInput, Value RatedPowerInput, Value InputPhase) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricalBaseProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"EnergySequence", EnergySequence}, {"UserDefinedEnergySequence", UserDefinedEnergySequence}, {"ElectricCurrentType", ElectricCurrentType}, {"InputVoltage", InputVoltage}, {"InputFrequency", InputFrequency}, {"FullLoadCurrent", FullLoadCurrent}, {"MinimumCircuitCurrent", MinimumCircuitCurrent}, {"MaximumPowerInput", MaximumPowerInput}, {"RatedPowerInput", RatedPowerInput}, {"InputPhase", InputPhase}}); }
inline Value IfcElectricalCircuit(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricalCircuit", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcElectricalElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcElectricalElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcElementAssembly(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value AssemblyPlace, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcElementAssembly", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"AssemblyPlace", AssemblyPlace}, {"PredefinedType", PredefinedType}}); }
inline Value IfcElementComponent(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcElementComponent", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcElementComponentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcElementComponentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcElementQuantity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value MethodOfMeasurement, Value Quantities) { return ifcapi::express::make_entity("IFC2X3", "IfcElementQuantity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"MethodOfMeasurement", MethodOfMeasurement}, {"Quantities", Quantities}}); }
inline Value IfcElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcElementarySurface(Value Position) { return ifcapi::express::make_entity("IFC2X3", "IfcElementarySurface", {{"Position", Position}}); }
inline Value IfcEllipse(Value Position, Value SemiAxis1, Value SemiAxis2) { return ifcapi::express::make_entity("IFC2X3", "IfcEllipse", {{"Position", Position}, {"SemiAxis1", SemiAxis1}, {"SemiAxis2", SemiAxis2}}); }
inline Value IfcEllipseProfileDef(Value ProfileType, Value ProfileName, Value Position, Value SemiAxis1, Value SemiAxis2) { return ifcapi::express::make_entity("IFC2X3", "IfcEllipseProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"SemiAxis1", SemiAxis1}, {"SemiAxis2", SemiAxis2}}); }
inline Value IfcEnergyConversionDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcEnergyConversionDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcEnergyConversionDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcEnergyConversionDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcEnergyProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value EnergySequence, Value UserDefinedEnergySequence) { return ifcapi::express::make_entity("IFC2X3", "IfcEnergyProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"EnergySequence", EnergySequence}, {"UserDefinedEnergySequence", UserDefinedEnergySequence}}); }
inline Value IfcEnvironmentalImpactValue(Value Name, Value Description, Value AppliedValue, Value UnitBasis, Value ApplicableDate, Value FixedUntilDate, Value ImpactType, Value Category, Value UserDefinedCategory) { return ifcapi::express::make_entity("IFC2X3", "IfcEnvironmentalImpactValue", {{"Name", Name}, {"Description", Description}, {"AppliedValue", AppliedValue}, {"UnitBasis", UnitBasis}, {"ApplicableDate", ApplicableDate}, {"FixedUntilDate", FixedUntilDate}, {"ImpactType", ImpactType}, {"Category", Category}, {"UserDefinedCategory", UserDefinedCategory}}); }
inline Value IfcEquipmentElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcEquipmentElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcEquipmentStandard(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcEquipmentStandard", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcEvaporativeCoolerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcEvaporativeCoolerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcEvaporatorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcEvaporatorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcExtendedMaterialProperties(Value Material, Value ExtendedProperties, Value Description, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcExtendedMaterialProperties", {{"Material", Material}, {"ExtendedProperties", ExtendedProperties}, {"Description", Description}, {"Name", Name}}); }
inline Value IfcExternalReference(Value Location, Value ItemReference, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcExternalReference", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}}); }
inline Value IfcExternallyDefinedHatchStyle(Value Location, Value ItemReference, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcExternallyDefinedHatchStyle", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}}); }
inline Value IfcExternallyDefinedSurfaceStyle(Value Location, Value ItemReference, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcExternallyDefinedSurfaceStyle", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}}); }
inline Value IfcExternallyDefinedSymbol(Value Location, Value ItemReference, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcExternallyDefinedSymbol", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}}); }
inline Value IfcExternallyDefinedTextFont(Value Location, Value ItemReference, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcExternallyDefinedTextFont", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}}); }
inline Value IfcExtrudedAreaSolid(Value SweptArea, Value Position, Value ExtrudedDirection, Value Depth) { return ifcapi::express::make_entity("IFC2X3", "IfcExtrudedAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}, {"ExtrudedDirection", ExtrudedDirection}, {"Depth", Depth}}); }
inline Value IfcFace(Value Bounds) { return ifcapi::express::make_entity("IFC2X3", "IfcFace", {{"Bounds", Bounds}}); }
inline Value IfcFaceBasedSurfaceModel(Value FbsmFaces) { return ifcapi::express::make_entity("IFC2X3", "IfcFaceBasedSurfaceModel", {{"FbsmFaces", FbsmFaces}}); }
inline Value IfcFaceBound(Value Bound, Value Orientation) { return ifcapi::express::make_entity("IFC2X3", "IfcFaceBound", {{"Bound", Bound}, {"Orientation", Orientation}}); }
inline Value IfcFaceOuterBound(Value Bound, Value Orientation) { return ifcapi::express::make_entity("IFC2X3", "IfcFaceOuterBound", {{"Bound", Bound}, {"Orientation", Orientation}}); }
inline Value IfcFaceSurface(Value Bounds, Value FaceSurface, Value SameSense) { return ifcapi::express::make_entity("IFC2X3", "IfcFaceSurface", {{"Bounds", Bounds}, {"FaceSurface", FaceSurface}, {"SameSense", SameSense}}); }
inline Value IfcFacetedBrep(Value Outer) { return ifcapi::express::make_entity("IFC2X3", "IfcFacetedBrep", {{"Outer", Outer}}); }
inline Value IfcFacetedBrepWithVoids(Value Outer, Value Voids) { return ifcapi::express::make_entity("IFC2X3", "IfcFacetedBrepWithVoids", {{"Outer", Outer}, {"Voids", Voids}}); }
inline Value IfcFailureConnectionCondition(Value Name, Value TensionFailureX, Value TensionFailureY, Value TensionFailureZ, Value CompressionFailureX, Value CompressionFailureY, Value CompressionFailureZ) { return ifcapi::express::make_entity("IFC2X3", "IfcFailureConnectionCondition", {{"Name", Name}, {"TensionFailureX", TensionFailureX}, {"TensionFailureY", TensionFailureY}, {"TensionFailureZ", TensionFailureZ}, {"CompressionFailureX", CompressionFailureX}, {"CompressionFailureY", CompressionFailureY}, {"CompressionFailureZ", CompressionFailureZ}}); }
inline Value IfcFanType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcFanType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFastener(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFastener", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFastenerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFastenerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFeatureElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFeatureElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFeatureElementAddition(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFeatureElementAddition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFeatureElementSubtraction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFeatureElementSubtraction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFillAreaStyle(Value Name, Value FillStyles) { return ifcapi::express::make_entity("IFC2X3", "IfcFillAreaStyle", {{"Name", Name}, {"FillStyles", FillStyles}}); }
inline Value IfcFillAreaStyleHatching(Value HatchLineAppearance, Value StartOfNextHatchLine, Value PointOfReferenceHatchLine, Value PatternStart, Value HatchLineAngle) { return ifcapi::express::make_entity("IFC2X3", "IfcFillAreaStyleHatching", {{"HatchLineAppearance", HatchLineAppearance}, {"StartOfNextHatchLine", StartOfNextHatchLine}, {"PointOfReferenceHatchLine", PointOfReferenceHatchLine}, {"PatternStart", PatternStart}, {"HatchLineAngle", HatchLineAngle}}); }
inline Value IfcFillAreaStyleTileSymbolWithStyle(Value Symbol) { return ifcapi::express::make_entity("IFC2X3", "IfcFillAreaStyleTileSymbolWithStyle", {{"Symbol", Symbol}}); }
inline Value IfcFillAreaStyleTiles(Value TilingPattern, Value Tiles, Value TilingScale) { return ifcapi::express::make_entity("IFC2X3", "IfcFillAreaStyleTiles", {{"TilingPattern", TilingPattern}, {"Tiles", Tiles}, {"TilingScale", TilingScale}}); }
inline Value IfcFilterType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcFilterType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFireSuppressionTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcFireSuppressionTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFlowController(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowController", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowControllerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowControllerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowFitting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowFitting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowInstrumentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowInstrumentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFlowMeterType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowMeterType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFlowMovingDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowMovingDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowMovingDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowMovingDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowSegment(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowSegment", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowStorageDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowStorageDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowStorageDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowStorageDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowTerminal(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowTerminal", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFlowTreatmentDevice(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowTreatmentDevice", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFlowTreatmentDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFlowTreatmentDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFluidFlowProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value PropertySource, Value FlowConditionTimeSeries, Value VelocityTimeSeries, Value FlowrateTimeSeries, Value Fluid, Value PressureTimeSeries, Value UserDefinedPropertySource, Value TemperatureSingleValue, Value WetBulbTemperatureSingleValue, Value WetBulbTemperatureTimeSeries, Value TemperatureTimeSeries, Value FlowrateSingleValue, Value FlowConditionSingleValue, Value VelocitySingleValue, Value PressureSingleValue) { return ifcapi::express::make_entity("IFC2X3", "IfcFluidFlowProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"PropertySource", PropertySource}, {"FlowConditionTimeSeries", FlowConditionTimeSeries}, {"VelocityTimeSeries", VelocityTimeSeries}, {"FlowrateTimeSeries", FlowrateTimeSeries}, {"Fluid", Fluid}, {"PressureTimeSeries", PressureTimeSeries}, {"UserDefinedPropertySource", UserDefinedPropertySource}, {"TemperatureSingleValue", TemperatureSingleValue}, {"WetBulbTemperatureSingleValue", WetBulbTemperatureSingleValue}, {"WetBulbTemperatureTimeSeries", WetBulbTemperatureTimeSeries}, {"TemperatureTimeSeries", TemperatureTimeSeries}, {"FlowrateSingleValue", FlowrateSingleValue}, {"FlowConditionSingleValue", FlowConditionSingleValue}, {"VelocitySingleValue", VelocitySingleValue}, {"PressureSingleValue", PressureSingleValue}}); }
inline Value IfcFooting(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcFooting", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcFuelProperties(Value Material, Value CombustionTemperature, Value CarbonContent, Value LowerHeatingValue, Value HigherHeatingValue) { return ifcapi::express::make_entity("IFC2X3", "IfcFuelProperties", {{"Material", Material}, {"CombustionTemperature", CombustionTemperature}, {"CarbonContent", CarbonContent}, {"LowerHeatingValue", LowerHeatingValue}, {"HigherHeatingValue", HigherHeatingValue}}); }
inline Value IfcFurnishingElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcFurnishingElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcFurnishingElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcFurnishingElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcFurnitureStandard(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcFurnitureStandard", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcFurnitureType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value AssemblyPlace) { return ifcapi::express::make_entity("IFC2X3", "IfcFurnitureType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"AssemblyPlace", AssemblyPlace}}); }
inline Value IfcGasTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcGasTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcGeneralMaterialProperties(Value Material, Value MolecularWeight, Value Porosity, Value MassDensity) { return ifcapi::express::make_entity("IFC2X3", "IfcGeneralMaterialProperties", {{"Material", Material}, {"MolecularWeight", MolecularWeight}, {"Porosity", Porosity}, {"MassDensity", MassDensity}}); }
inline Value IfcGeneralProfileProperties(Value ProfileName, Value ProfileDefinition, Value PhysicalWeight, Value Perimeter, Value MinimumPlateThickness, Value MaximumPlateThickness, Value CrossSectionArea) { return ifcapi::express::make_entity("IFC2X3", "IfcGeneralProfileProperties", {{"ProfileName", ProfileName}, {"ProfileDefinition", ProfileDefinition}, {"PhysicalWeight", PhysicalWeight}, {"Perimeter", Perimeter}, {"MinimumPlateThickness", MinimumPlateThickness}, {"MaximumPlateThickness", MaximumPlateThickness}, {"CrossSectionArea", CrossSectionArea}}); }
inline Value IfcGeometricCurveSet(Value Elements) { return ifcapi::express::make_entity("IFC2X3", "IfcGeometricCurveSet", {{"Elements", Elements}}); }
inline Value IfcGeometricRepresentationContext(Value ContextIdentifier, Value ContextType, Value CoordinateSpaceDimension, Value Precision, Value WorldCoordinateSystem, Value TrueNorth) { return ifcapi::express::make_entity("IFC2X3", "IfcGeometricRepresentationContext", {{"ContextIdentifier", ContextIdentifier}, {"ContextType", ContextType}, {"CoordinateSpaceDimension", CoordinateSpaceDimension}, {"Precision", Precision}, {"WorldCoordinateSystem", WorldCoordinateSystem}, {"TrueNorth", TrueNorth}}); }
inline Value IfcGeometricRepresentationItem() { return ifcapi::express::make_entity("IFC2X3", "IfcGeometricRepresentationItem", {}); }
inline Value IfcGeometricRepresentationSubContext(Value ContextIdentifier, Value ContextType, Value CoordinateSpaceDimension, Value Precision, Value WorldCoordinateSystem, Value TrueNorth, Value ParentContext, Value TargetScale, Value TargetView, Value UserDefinedTargetView) { return ifcapi::express::make_entity("IFC2X3", "IfcGeometricRepresentationSubContext", {{"ContextIdentifier", ContextIdentifier}, {"ContextType", ContextType}, {"CoordinateSpaceDimension", CoordinateSpaceDimension}, {"Precision", Precision}, {"WorldCoordinateSystem", WorldCoordinateSystem}, {"TrueNorth", TrueNorth}, {"ParentContext", ParentContext}, {"TargetScale", TargetScale}, {"TargetView", TargetView}, {"UserDefinedTargetView", UserDefinedTargetView}}); }
inline Value IfcGeometricSet(Value Elements) { return ifcapi::express::make_entity("IFC2X3", "IfcGeometricSet", {{"Elements", Elements}}); }
inline Value IfcGrid(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value UAxes, Value VAxes, Value WAxes) { return ifcapi::express::make_entity("IFC2X3", "IfcGrid", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"UAxes", UAxes}, {"VAxes", VAxes}, {"WAxes", WAxes}}); }
inline Value IfcGridAxis(Value AxisTag, Value AxisCurve, Value SameSense) { return ifcapi::express::make_entity("IFC2X3", "IfcGridAxis", {{"AxisTag", AxisTag}, {"AxisCurve", AxisCurve}, {"SameSense", SameSense}}); }
inline Value IfcGridPlacement(Value PlacementLocation, Value PlacementRefDirection) { return ifcapi::express::make_entity("IFC2X3", "IfcGridPlacement", {{"PlacementLocation", PlacementLocation}, {"PlacementRefDirection", PlacementRefDirection}}); }
inline Value IfcGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcHalfSpaceSolid(Value BaseSurface, Value AgreementFlag) { return ifcapi::express::make_entity("IFC2X3", "IfcHalfSpaceSolid", {{"BaseSurface", BaseSurface}, {"AgreementFlag", AgreementFlag}}); }
inline Value IfcHeatExchangerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcHeatExchangerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcHumidifierType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcHumidifierType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcHygroscopicMaterialProperties(Value Material, Value UpperVaporResistanceFactor, Value LowerVaporResistanceFactor, Value IsothermalMoistureCapacity, Value VaporPermeability, Value MoistureDiffusivity) { return ifcapi::express::make_entity("IFC2X3", "IfcHygroscopicMaterialProperties", {{"Material", Material}, {"UpperVaporResistanceFactor", UpperVaporResistanceFactor}, {"LowerVaporResistanceFactor", LowerVaporResistanceFactor}, {"IsothermalMoistureCapacity", IsothermalMoistureCapacity}, {"VaporPermeability", VaporPermeability}, {"MoistureDiffusivity", MoistureDiffusivity}}); }
inline Value IfcIShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value OverallWidth, Value OverallDepth, Value WebThickness, Value FlangeThickness, Value FilletRadius) { return ifcapi::express::make_entity("IFC2X3", "IfcIShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"OverallWidth", OverallWidth}, {"OverallDepth", OverallDepth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}}); }
inline Value IfcImageTexture(Value RepeatS, Value RepeatT, Value TextureType, Value TextureTransform, Value UrlReference) { return ifcapi::express::make_entity("IFC2X3", "IfcImageTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"TextureType", TextureType}, {"TextureTransform", TextureTransform}, {"UrlReference", UrlReference}}); }
inline Value IfcInventory(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value InventoryType, Value Jurisdiction, Value ResponsiblePersons, Value LastUpdateDate, Value CurrentValue, Value OriginalValue) { return ifcapi::express::make_entity("IFC2X3", "IfcInventory", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"InventoryType", InventoryType}, {"Jurisdiction", Jurisdiction}, {"ResponsiblePersons", ResponsiblePersons}, {"LastUpdateDate", LastUpdateDate}, {"CurrentValue", CurrentValue}, {"OriginalValue", OriginalValue}}); }
inline Value IfcIrregularTimeSeries(Value Name, Value Description, Value StartTime, Value EndTime, Value TimeSeriesDataType, Value DataOrigin, Value UserDefinedDataOrigin, Value Unit, Value Values) { return ifcapi::express::make_entity("IFC2X3", "IfcIrregularTimeSeries", {{"Name", Name}, {"Description", Description}, {"StartTime", StartTime}, {"EndTime", EndTime}, {"TimeSeriesDataType", TimeSeriesDataType}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"Unit", Unit}, {"Values", Values}}); }
inline Value IfcIrregularTimeSeriesValue(Value TimeStamp, Value ListValues) { return ifcapi::express::make_entity("IFC2X3", "IfcIrregularTimeSeriesValue", {{"TimeStamp", TimeStamp}, {"ListValues", ListValues}}); }
inline Value IfcJunctionBoxType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcJunctionBoxType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value Width, Value Thickness, Value FilletRadius, Value EdgeRadius, Value LegSlope, Value CentreOfGravityInX, Value CentreOfGravityInY) { return ifcapi::express::make_entity("IFC2X3", "IfcLShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"Width", Width}, {"Thickness", Thickness}, {"FilletRadius", FilletRadius}, {"EdgeRadius", EdgeRadius}, {"LegSlope", LegSlope}, {"CentreOfGravityInX", CentreOfGravityInX}, {"CentreOfGravityInY", CentreOfGravityInY}}); }
inline Value IfcLaborResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ResourceIdentifier, Value ResourceGroup, Value ResourceConsumption, Value BaseQuantity, Value SkillSet) { return ifcapi::express::make_entity("IFC2X3", "IfcLaborResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ResourceIdentifier", ResourceIdentifier}, {"ResourceGroup", ResourceGroup}, {"ResourceConsumption", ResourceConsumption}, {"BaseQuantity", BaseQuantity}, {"SkillSet", SkillSet}}); }
inline Value IfcLampType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcLampType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLibraryInformation(Value Name, Value Version, Value Publisher, Value VersionDate, Value LibraryReference) { return ifcapi::express::make_entity("IFC2X3", "IfcLibraryInformation", {{"Name", Name}, {"Version", Version}, {"Publisher", Publisher}, {"VersionDate", VersionDate}, {"LibraryReference", LibraryReference}}); }
inline Value IfcLibraryReference(Value Location, Value ItemReference, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcLibraryReference", {{"Location", Location}, {"ItemReference", ItemReference}, {"Name", Name}}); }
inline Value IfcLightDistributionData(Value MainPlaneAngle, Value SecondaryPlaneAngle, Value LuminousIntensity) { return ifcapi::express::make_entity("IFC2X3", "IfcLightDistributionData", {{"MainPlaneAngle", MainPlaneAngle}, {"SecondaryPlaneAngle", SecondaryPlaneAngle}, {"LuminousIntensity", LuminousIntensity}}); }
inline Value IfcLightFixtureType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcLightFixtureType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcLightIntensityDistribution(Value LightDistributionCurve, Value DistributionData) { return ifcapi::express::make_entity("IFC2X3", "IfcLightIntensityDistribution", {{"LightDistributionCurve", LightDistributionCurve}, {"DistributionData", DistributionData}}); }
inline Value IfcLightSource(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity) { return ifcapi::express::make_entity("IFC2X3", "IfcLightSource", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}}); }
inline Value IfcLightSourceAmbient(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity) { return ifcapi::express::make_entity("IFC2X3", "IfcLightSourceAmbient", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}}); }
inline Value IfcLightSourceDirectional(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Orientation) { return ifcapi::express::make_entity("IFC2X3", "IfcLightSourceDirectional", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Orientation", Orientation}}); }
inline Value IfcLightSourceGoniometric(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Position, Value ColourAppearance, Value ColourTemperature, Value LuminousFlux, Value LightEmissionSource, Value LightDistributionDataSource) { return ifcapi::express::make_entity("IFC2X3", "IfcLightSourceGoniometric", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Position", Position}, {"ColourAppearance", ColourAppearance}, {"ColourTemperature", ColourTemperature}, {"LuminousFlux", LuminousFlux}, {"LightEmissionSource", LightEmissionSource}, {"LightDistributionDataSource", LightDistributionDataSource}}); }
inline Value IfcLightSourcePositional(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Position, Value Radius, Value ConstantAttenuation, Value DistanceAttenuation, Value QuadricAttenuation) { return ifcapi::express::make_entity("IFC2X3", "IfcLightSourcePositional", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Position", Position}, {"Radius", Radius}, {"ConstantAttenuation", ConstantAttenuation}, {"DistanceAttenuation", DistanceAttenuation}, {"QuadricAttenuation", QuadricAttenuation}}); }
inline Value IfcLightSourceSpot(Value Name, Value LightColour, Value AmbientIntensity, Value Intensity, Value Position, Value Radius, Value ConstantAttenuation, Value DistanceAttenuation, Value QuadricAttenuation, Value Orientation, Value ConcentrationExponent, Value SpreadAngle, Value BeamWidthAngle) { return ifcapi::express::make_entity("IFC2X3", "IfcLightSourceSpot", {{"Name", Name}, {"LightColour", LightColour}, {"AmbientIntensity", AmbientIntensity}, {"Intensity", Intensity}, {"Position", Position}, {"Radius", Radius}, {"ConstantAttenuation", ConstantAttenuation}, {"DistanceAttenuation", DistanceAttenuation}, {"QuadricAttenuation", QuadricAttenuation}, {"Orientation", Orientation}, {"ConcentrationExponent", ConcentrationExponent}, {"SpreadAngle", SpreadAngle}, {"BeamWidthAngle", BeamWidthAngle}}); }
inline Value IfcLine(Value Pnt, Value Dir) { return ifcapi::express::make_entity("IFC2X3", "IfcLine", {{"Pnt", Pnt}, {"Dir", Dir}}); }
inline Value IfcLinearDimension(Value Contents) { return ifcapi::express::make_entity("IFC2X3", "IfcLinearDimension", {{"Contents", Contents}}); }
inline Value IfcLocalPlacement(Value PlacementRelTo, Value RelativePlacement) { return ifcapi::express::make_entity("IFC2X3", "IfcLocalPlacement", {{"PlacementRelTo", PlacementRelTo}, {"RelativePlacement", RelativePlacement}}); }
inline Value IfcLocalTime(Value HourComponent, Value MinuteComponent, Value SecondComponent, Value Zone, Value DaylightSavingOffset) { return ifcapi::express::make_entity("IFC2X3", "IfcLocalTime", {{"HourComponent", HourComponent}, {"MinuteComponent", MinuteComponent}, {"SecondComponent", SecondComponent}, {"Zone", Zone}, {"DaylightSavingOffset", DaylightSavingOffset}}); }
inline Value IfcLoop() { return ifcapi::express::make_entity("IFC2X3", "IfcLoop", {}); }
inline Value IfcManifoldSolidBrep(Value Outer) { return ifcapi::express::make_entity("IFC2X3", "IfcManifoldSolidBrep", {{"Outer", Outer}}); }
inline Value IfcMappedItem(Value MappingSource, Value MappingTarget) { return ifcapi::express::make_entity("IFC2X3", "IfcMappedItem", {{"MappingSource", MappingSource}, {"MappingTarget", MappingTarget}}); }
inline Value IfcMaterial(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterial", {{"Name", Name}}); }
inline Value IfcMaterialClassificationRelationship(Value MaterialClassifications, Value ClassifiedMaterial) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterialClassificationRelationship", {{"MaterialClassifications", MaterialClassifications}, {"ClassifiedMaterial", ClassifiedMaterial}}); }
inline Value IfcMaterialDefinitionRepresentation(Value Name, Value Description, Value Representations, Value RepresentedMaterial) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterialDefinitionRepresentation", {{"Name", Name}, {"Description", Description}, {"Representations", Representations}, {"RepresentedMaterial", RepresentedMaterial}}); }
inline Value IfcMaterialLayer(Value Material, Value LayerThickness, Value IsVentilated) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterialLayer", {{"Material", Material}, {"LayerThickness", LayerThickness}, {"IsVentilated", IsVentilated}}); }
inline Value IfcMaterialLayerSet(Value MaterialLayers, Value LayerSetName) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterialLayerSet", {{"MaterialLayers", MaterialLayers}, {"LayerSetName", LayerSetName}}); }
inline Value IfcMaterialLayerSetUsage(Value ForLayerSet, Value LayerSetDirection, Value DirectionSense, Value OffsetFromReferenceLine) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterialLayerSetUsage", {{"ForLayerSet", ForLayerSet}, {"LayerSetDirection", LayerSetDirection}, {"DirectionSense", DirectionSense}, {"OffsetFromReferenceLine", OffsetFromReferenceLine}}); }
inline Value IfcMaterialList(Value Materials) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterialList", {{"Materials", Materials}}); }
inline Value IfcMaterialProperties(Value Material) { return ifcapi::express::make_entity("IFC2X3", "IfcMaterialProperties", {{"Material", Material}}); }
inline Value IfcMeasureWithUnit(Value ValueComponent, Value UnitComponent) { return ifcapi::express::make_entity("IFC2X3", "IfcMeasureWithUnit", {{"ValueComponent", ValueComponent}, {"UnitComponent", UnitComponent}}); }
inline Value IfcMechanicalConcreteMaterialProperties(Value Material, Value DynamicViscosity, Value YoungModulus, Value ShearModulus, Value PoissonRatio, Value ThermalExpansionCoefficient, Value CompressiveStrength, Value MaxAggregateSize, Value AdmixturesDescription, Value Workability, Value ProtectivePoreRatio, Value WaterImpermeability) { return ifcapi::express::make_entity("IFC2X3", "IfcMechanicalConcreteMaterialProperties", {{"Material", Material}, {"DynamicViscosity", DynamicViscosity}, {"YoungModulus", YoungModulus}, {"ShearModulus", ShearModulus}, {"PoissonRatio", PoissonRatio}, {"ThermalExpansionCoefficient", ThermalExpansionCoefficient}, {"CompressiveStrength", CompressiveStrength}, {"MaxAggregateSize", MaxAggregateSize}, {"AdmixturesDescription", AdmixturesDescription}, {"Workability", Workability}, {"ProtectivePoreRatio", ProtectivePoreRatio}, {"WaterImpermeability", WaterImpermeability}}); }
inline Value IfcMechanicalFastener(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value NominalDiameter, Value NominalLength) { return ifcapi::express::make_entity("IFC2X3", "IfcMechanicalFastener", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"NominalDiameter", NominalDiameter}, {"NominalLength", NominalLength}}); }
inline Value IfcMechanicalFastenerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcMechanicalFastenerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcMechanicalMaterialProperties(Value Material, Value DynamicViscosity, Value YoungModulus, Value ShearModulus, Value PoissonRatio, Value ThermalExpansionCoefficient) { return ifcapi::express::make_entity("IFC2X3", "IfcMechanicalMaterialProperties", {{"Material", Material}, {"DynamicViscosity", DynamicViscosity}, {"YoungModulus", YoungModulus}, {"ShearModulus", ShearModulus}, {"PoissonRatio", PoissonRatio}, {"ThermalExpansionCoefficient", ThermalExpansionCoefficient}}); }
inline Value IfcMechanicalSteelMaterialProperties(Value Material, Value DynamicViscosity, Value YoungModulus, Value ShearModulus, Value PoissonRatio, Value ThermalExpansionCoefficient, Value YieldStress, Value UltimateStress, Value UltimateStrain, Value HardeningModule, Value ProportionalStress, Value PlasticStrain, Value Relaxations) { return ifcapi::express::make_entity("IFC2X3", "IfcMechanicalSteelMaterialProperties", {{"Material", Material}, {"DynamicViscosity", DynamicViscosity}, {"YoungModulus", YoungModulus}, {"ShearModulus", ShearModulus}, {"PoissonRatio", PoissonRatio}, {"ThermalExpansionCoefficient", ThermalExpansionCoefficient}, {"YieldStress", YieldStress}, {"UltimateStress", UltimateStress}, {"UltimateStrain", UltimateStrain}, {"HardeningModule", HardeningModule}, {"ProportionalStress", ProportionalStress}, {"PlasticStrain", PlasticStrain}, {"Relaxations", Relaxations}}); }
inline Value IfcMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcMemberType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcMemberType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMetric(Value Name, Value Description, Value ConstraintGrade, Value ConstraintSource, Value CreatingActor, Value CreationTime, Value UserDefinedGrade, Value Benchmark, Value ValueSource, Value DataValue) { return ifcapi::express::make_entity("IFC2X3", "IfcMetric", {{"Name", Name}, {"Description", Description}, {"ConstraintGrade", ConstraintGrade}, {"ConstraintSource", ConstraintSource}, {"CreatingActor", CreatingActor}, {"CreationTime", CreationTime}, {"UserDefinedGrade", UserDefinedGrade}, {"Benchmark", Benchmark}, {"ValueSource", ValueSource}, {"DataValue", DataValue}}); }
inline Value IfcMonetaryUnit(Value Currency) { return ifcapi::express::make_entity("IFC2X3", "IfcMonetaryUnit", {{"Currency", Currency}}); }
inline Value IfcMotorConnectionType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcMotorConnectionType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcMove(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TaskId, Value Status, Value WorkMethod, Value IsMilestone, Value Priority, Value MoveFrom, Value MoveTo, Value PunchList) { return ifcapi::express::make_entity("IFC2X3", "IfcMove", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TaskId", TaskId}, {"Status", Status}, {"WorkMethod", WorkMethod}, {"IsMilestone", IsMilestone}, {"Priority", Priority}, {"MoveFrom", MoveFrom}, {"MoveTo", MoveTo}, {"PunchList", PunchList}}); }
inline Value IfcNamedUnit(Value Dimensions, Value UnitType) { return ifcapi::express::make_entity("IFC2X3", "IfcNamedUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}}); }
inline Value IfcObject(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcObject", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcObjectDefinition(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcObjectDefinition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcObjectPlacement() { return ifcapi::express::make_entity("IFC2X3", "IfcObjectPlacement", {}); }
inline Value IfcObjective(Value Name, Value Description, Value ConstraintGrade, Value ConstraintSource, Value CreatingActor, Value CreationTime, Value UserDefinedGrade, Value BenchmarkValues, Value ResultValues, Value ObjectiveQualifier, Value UserDefinedQualifier) { return ifcapi::express::make_entity("IFC2X3", "IfcObjective", {{"Name", Name}, {"Description", Description}, {"ConstraintGrade", ConstraintGrade}, {"ConstraintSource", ConstraintSource}, {"CreatingActor", CreatingActor}, {"CreationTime", CreationTime}, {"UserDefinedGrade", UserDefinedGrade}, {"BenchmarkValues", BenchmarkValues}, {"ResultValues", ResultValues}, {"ObjectiveQualifier", ObjectiveQualifier}, {"UserDefinedQualifier", UserDefinedQualifier}}); }
inline Value IfcOccupant(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TheActor, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcOccupant", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TheActor", TheActor}, {"PredefinedType", PredefinedType}}); }
inline Value IfcOffsetCurve2D(Value BasisCurve, Value Distance, Value SelfIntersect) { return ifcapi::express::make_entity("IFC2X3", "IfcOffsetCurve2D", {{"BasisCurve", BasisCurve}, {"Distance", Distance}, {"SelfIntersect", SelfIntersect}}); }
inline Value IfcOffsetCurve3D(Value BasisCurve, Value Distance, Value SelfIntersect, Value RefDirection) { return ifcapi::express::make_entity("IFC2X3", "IfcOffsetCurve3D", {{"BasisCurve", BasisCurve}, {"Distance", Distance}, {"SelfIntersect", SelfIntersect}, {"RefDirection", RefDirection}}); }
inline Value IfcOneDirectionRepeatFactor(Value RepeatFactor) { return ifcapi::express::make_entity("IFC2X3", "IfcOneDirectionRepeatFactor", {{"RepeatFactor", RepeatFactor}}); }
inline Value IfcOpenShell(Value CfsFaces) { return ifcapi::express::make_entity("IFC2X3", "IfcOpenShell", {{"CfsFaces", CfsFaces}}); }
inline Value IfcOpeningElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcOpeningElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcOpticalMaterialProperties(Value Material, Value VisibleTransmittance, Value SolarTransmittance, Value ThermalIrTransmittance, Value ThermalIrEmissivityBack, Value ThermalIrEmissivityFront, Value VisibleReflectanceBack, Value VisibleReflectanceFront, Value SolarReflectanceFront, Value SolarReflectanceBack) { return ifcapi::express::make_entity("IFC2X3", "IfcOpticalMaterialProperties", {{"Material", Material}, {"VisibleTransmittance", VisibleTransmittance}, {"SolarTransmittance", SolarTransmittance}, {"ThermalIrTransmittance", ThermalIrTransmittance}, {"ThermalIrEmissivityBack", ThermalIrEmissivityBack}, {"ThermalIrEmissivityFront", ThermalIrEmissivityFront}, {"VisibleReflectanceBack", VisibleReflectanceBack}, {"VisibleReflectanceFront", VisibleReflectanceFront}, {"SolarReflectanceFront", SolarReflectanceFront}, {"SolarReflectanceBack", SolarReflectanceBack}}); }
inline Value IfcOrderAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TaskId, Value Status, Value WorkMethod, Value IsMilestone, Value Priority, Value ActionID) { return ifcapi::express::make_entity("IFC2X3", "IfcOrderAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TaskId", TaskId}, {"Status", Status}, {"WorkMethod", WorkMethod}, {"IsMilestone", IsMilestone}, {"Priority", Priority}, {"ActionID", ActionID}}); }
inline Value IfcOrganization(Value Id, Value Name, Value Description, Value Roles, Value Addresses) { return ifcapi::express::make_entity("IFC2X3", "IfcOrganization", {{"Id", Id}, {"Name", Name}, {"Description", Description}, {"Roles", Roles}, {"Addresses", Addresses}}); }
inline Value IfcOrganizationRelationship(Value Name, Value Description, Value RelatingOrganization, Value RelatedOrganizations) { return ifcapi::express::make_entity("IFC2X3", "IfcOrganizationRelationship", {{"Name", Name}, {"Description", Description}, {"RelatingOrganization", RelatingOrganization}, {"RelatedOrganizations", RelatedOrganizations}}); }
inline Value IfcOrientedEdge(Value EdgeStart, Value EdgeEnd, Value EdgeElement, Value Orientation) { return ifcapi::express::make_entity("IFC2X3", "IfcOrientedEdge", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}, {"EdgeElement", EdgeElement}, {"Orientation", Orientation}}); }
inline Value IfcOutletType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcOutletType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcOwnerHistory(Value OwningUser, Value OwningApplication, Value State, Value ChangeAction, Value LastModifiedDate, Value LastModifyingUser, Value LastModifyingApplication, Value CreationDate) { return ifcapi::express::make_entity("IFC2X3", "IfcOwnerHistory", {{"OwningUser", OwningUser}, {"OwningApplication", OwningApplication}, {"State", State}, {"ChangeAction", ChangeAction}, {"LastModifiedDate", LastModifiedDate}, {"LastModifyingUser", LastModifyingUser}, {"LastModifyingApplication", LastModifyingApplication}, {"CreationDate", CreationDate}}); }
inline Value IfcParameterizedProfileDef(Value ProfileType, Value ProfileName, Value Position) { return ifcapi::express::make_entity("IFC2X3", "IfcParameterizedProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}}); }
inline Value IfcPath(Value EdgeList) { return ifcapi::express::make_entity("IFC2X3", "IfcPath", {{"EdgeList", EdgeList}}); }
inline Value IfcPerformanceHistory(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LifeCyclePhase) { return ifcapi::express::make_entity("IFC2X3", "IfcPerformanceHistory", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LifeCyclePhase", LifeCyclePhase}}); }
inline Value IfcPermeableCoveringProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value OperationType, Value PanelPosition, Value FrameDepth, Value FrameThickness, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC2X3", "IfcPermeableCoveringProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"OperationType", OperationType}, {"PanelPosition", PanelPosition}, {"FrameDepth", FrameDepth}, {"FrameThickness", FrameThickness}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcPermit(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PermitID) { return ifcapi::express::make_entity("IFC2X3", "IfcPermit", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PermitID", PermitID}}); }
inline Value IfcPerson(Value Id, Value FamilyName, Value GivenName, Value MiddleNames, Value PrefixTitles, Value SuffixTitles, Value Roles, Value Addresses) { return ifcapi::express::make_entity("IFC2X3", "IfcPerson", {{"Id", Id}, {"FamilyName", FamilyName}, {"GivenName", GivenName}, {"MiddleNames", MiddleNames}, {"PrefixTitles", PrefixTitles}, {"SuffixTitles", SuffixTitles}, {"Roles", Roles}, {"Addresses", Addresses}}); }
inline Value IfcPersonAndOrganization(Value ThePerson, Value TheOrganization, Value Roles) { return ifcapi::express::make_entity("IFC2X3", "IfcPersonAndOrganization", {{"ThePerson", ThePerson}, {"TheOrganization", TheOrganization}, {"Roles", Roles}}); }
inline Value IfcPhysicalComplexQuantity(Value Name, Value Description, Value HasQuantities, Value Discrimination, Value Quality, Value Usage) { return ifcapi::express::make_entity("IFC2X3", "IfcPhysicalComplexQuantity", {{"Name", Name}, {"Description", Description}, {"HasQuantities", HasQuantities}, {"Discrimination", Discrimination}, {"Quality", Quality}, {"Usage", Usage}}); }
inline Value IfcPhysicalQuantity(Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcPhysicalQuantity", {{"Name", Name}, {"Description", Description}}); }
inline Value IfcPhysicalSimpleQuantity(Value Name, Value Description, Value Unit) { return ifcapi::express::make_entity("IFC2X3", "IfcPhysicalSimpleQuantity", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}}); }
inline Value IfcPile(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType, Value ConstructionType) { return ifcapi::express::make_entity("IFC2X3", "IfcPile", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}, {"ConstructionType", ConstructionType}}); }
inline Value IfcPipeFittingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcPipeFittingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPipeSegmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcPipeSegmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPixelTexture(Value RepeatS, Value RepeatT, Value TextureType, Value TextureTransform, Value Width, Value Height, Value ColourComponents, Value Pixel) { return ifcapi::express::make_entity("IFC2X3", "IfcPixelTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"TextureType", TextureType}, {"TextureTransform", TextureTransform}, {"Width", Width}, {"Height", Height}, {"ColourComponents", ColourComponents}, {"Pixel", Pixel}}); }
inline Value IfcPlacement(Value Location) { return ifcapi::express::make_entity("IFC2X3", "IfcPlacement", {{"Location", Location}}); }
inline Value IfcPlanarBox(Value SizeInX, Value SizeInY, Value Placement) { return ifcapi::express::make_entity("IFC2X3", "IfcPlanarBox", {{"SizeInX", SizeInX}, {"SizeInY", SizeInY}, {"Placement", Placement}}); }
inline Value IfcPlanarExtent(Value SizeInX, Value SizeInY) { return ifcapi::express::make_entity("IFC2X3", "IfcPlanarExtent", {{"SizeInX", SizeInX}, {"SizeInY", SizeInY}}); }
inline Value IfcPlane(Value Position) { return ifcapi::express::make_entity("IFC2X3", "IfcPlane", {{"Position", Position}}); }
inline Value IfcPlate(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcPlate", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcPlateType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcPlateType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcPoint() { return ifcapi::express::make_entity("IFC2X3", "IfcPoint", {}); }
inline Value IfcPointOnCurve(Value BasisCurve, Value PointParameter) { return ifcapi::express::make_entity("IFC2X3", "IfcPointOnCurve", {{"BasisCurve", BasisCurve}, {"PointParameter", PointParameter}}); }
inline Value IfcPointOnSurface(Value BasisSurface, Value PointParameterU, Value PointParameterV) { return ifcapi::express::make_entity("IFC2X3", "IfcPointOnSurface", {{"BasisSurface", BasisSurface}, {"PointParameterU", PointParameterU}, {"PointParameterV", PointParameterV}}); }
inline Value IfcPolyLoop(Value Polygon) { return ifcapi::express::make_entity("IFC2X3", "IfcPolyLoop", {{"Polygon", Polygon}}); }
inline Value IfcPolygonalBoundedHalfSpace(Value BaseSurface, Value AgreementFlag, Value Position, Value PolygonalBoundary) { return ifcapi::express::make_entity("IFC2X3", "IfcPolygonalBoundedHalfSpace", {{"BaseSurface", BaseSurface}, {"AgreementFlag", AgreementFlag}, {"Position", Position}, {"PolygonalBoundary", PolygonalBoundary}}); }
inline Value IfcPolyline(Value Points) { return ifcapi::express::make_entity("IFC2X3", "IfcPolyline", {{"Points", Points}}); }
inline Value IfcPort(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC2X3", "IfcPort", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcPostalAddress(Value Purpose, Value Description, Value UserDefinedPurpose, Value InternalLocation, Value AddressLines, Value PostalBox, Value Town, Value Region, Value PostalCode, Value Country) { return ifcapi::express::make_entity("IFC2X3", "IfcPostalAddress", {{"Purpose", Purpose}, {"Description", Description}, {"UserDefinedPurpose", UserDefinedPurpose}, {"InternalLocation", InternalLocation}, {"AddressLines", AddressLines}, {"PostalBox", PostalBox}, {"Town", Town}, {"Region", Region}, {"PostalCode", PostalCode}, {"Country", Country}}); }
inline Value IfcPreDefinedColour(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedColour", {{"Name", Name}}); }
inline Value IfcPreDefinedCurveFont(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedCurveFont", {{"Name", Name}}); }
inline Value IfcPreDefinedDimensionSymbol(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedDimensionSymbol", {{"Name", Name}}); }
inline Value IfcPreDefinedItem(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedItem", {{"Name", Name}}); }
inline Value IfcPreDefinedPointMarkerSymbol(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedPointMarkerSymbol", {{"Name", Name}}); }
inline Value IfcPreDefinedSymbol(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedSymbol", {{"Name", Name}}); }
inline Value IfcPreDefinedTerminatorSymbol(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedTerminatorSymbol", {{"Name", Name}}); }
inline Value IfcPreDefinedTextFont(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPreDefinedTextFont", {{"Name", Name}}); }
inline Value IfcPresentationLayerAssignment(Value Name, Value Description, Value AssignedItems, Value Identifier) { return ifcapi::express::make_entity("IFC2X3", "IfcPresentationLayerAssignment", {{"Name", Name}, {"Description", Description}, {"AssignedItems", AssignedItems}, {"Identifier", Identifier}}); }
inline Value IfcPresentationLayerWithStyle(Value Name, Value Description, Value AssignedItems, Value Identifier, Value LayerOn, Value LayerFrozen, Value LayerBlocked, Value LayerStyles) { return ifcapi::express::make_entity("IFC2X3", "IfcPresentationLayerWithStyle", {{"Name", Name}, {"Description", Description}, {"AssignedItems", AssignedItems}, {"Identifier", Identifier}, {"LayerOn", LayerOn}, {"LayerFrozen", LayerFrozen}, {"LayerBlocked", LayerBlocked}, {"LayerStyles", LayerStyles}}); }
inline Value IfcPresentationStyle(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcPresentationStyle", {{"Name", Name}}); }
inline Value IfcPresentationStyleAssignment(Value Styles) { return ifcapi::express::make_entity("IFC2X3", "IfcPresentationStyleAssignment", {{"Styles", Styles}}); }
inline Value IfcProcedure(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ProcedureID, Value ProcedureType, Value UserDefinedProcedureType) { return ifcapi::express::make_entity("IFC2X3", "IfcProcedure", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ProcedureID", ProcedureID}, {"ProcedureType", ProcedureType}, {"UserDefinedProcedureType", UserDefinedProcedureType}}); }
inline Value IfcProcess(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcProcess", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcProduct(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC2X3", "IfcProduct", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcProductDefinitionShape(Value Name, Value Description, Value Representations) { return ifcapi::express::make_entity("IFC2X3", "IfcProductDefinitionShape", {{"Name", Name}, {"Description", Description}, {"Representations", Representations}}); }
inline Value IfcProductRepresentation(Value Name, Value Description, Value Representations) { return ifcapi::express::make_entity("IFC2X3", "IfcProductRepresentation", {{"Name", Name}, {"Description", Description}, {"Representations", Representations}}); }
inline Value IfcProductsOfCombustionProperties(Value Material, Value SpecificHeatCapacity, Value N20Content, Value COContent, Value CO2Content) { return ifcapi::express::make_entity("IFC2X3", "IfcProductsOfCombustionProperties", {{"Material", Material}, {"SpecificHeatCapacity", SpecificHeatCapacity}, {"N20Content", N20Content}, {"COContent", COContent}, {"CO2Content", CO2Content}}); }
inline Value IfcProfileDef(Value ProfileType, Value ProfileName) { return ifcapi::express::make_entity("IFC2X3", "IfcProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}}); }
inline Value IfcProfileProperties(Value ProfileName, Value ProfileDefinition) { return ifcapi::express::make_entity("IFC2X3", "IfcProfileProperties", {{"ProfileName", ProfileName}, {"ProfileDefinition", ProfileDefinition}}); }
inline Value IfcProject(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value LongName, Value Phase, Value RepresentationContexts, Value UnitsInContext) { return ifcapi::express::make_entity("IFC2X3", "IfcProject", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"LongName", LongName}, {"Phase", Phase}, {"RepresentationContexts", RepresentationContexts}, {"UnitsInContext", UnitsInContext}}); }
inline Value IfcProjectOrder(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ID, Value PredefinedType, Value Status) { return ifcapi::express::make_entity("IFC2X3", "IfcProjectOrder", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ID", ID}, {"PredefinedType", PredefinedType}, {"Status", Status}}); }
inline Value IfcProjectOrderRecord(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Records, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcProjectOrderRecord", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Records", Records}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProjectionCurve(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcProjectionCurve", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcProjectionElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcProjectionElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcProperty(Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcProperty", {{"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertyBoundedValue(Value Name, Value Description, Value UpperBoundValue, Value LowerBoundValue, Value Unit) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyBoundedValue", {{"Name", Name}, {"Description", Description}, {"UpperBoundValue", UpperBoundValue}, {"LowerBoundValue", LowerBoundValue}, {"Unit", Unit}}); }
inline Value IfcPropertyConstraintRelationship(Value RelatingConstraint, Value RelatedProperties, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyConstraintRelationship", {{"RelatingConstraint", RelatingConstraint}, {"RelatedProperties", RelatedProperties}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertyDefinition(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyDefinition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertyDependencyRelationship(Value DependingProperty, Value DependantProperty, Value Name, Value Description, Value Expression) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyDependencyRelationship", {{"DependingProperty", DependingProperty}, {"DependantProperty", DependantProperty}, {"Name", Name}, {"Description", Description}, {"Expression", Expression}}); }
inline Value IfcPropertyEnumeratedValue(Value Name, Value Description, Value EnumerationValues, Value EnumerationReference) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyEnumeratedValue", {{"Name", Name}, {"Description", Description}, {"EnumerationValues", EnumerationValues}, {"EnumerationReference", EnumerationReference}}); }
inline Value IfcPropertyEnumeration(Value Name, Value EnumerationValues, Value Unit) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyEnumeration", {{"Name", Name}, {"EnumerationValues", EnumerationValues}, {"Unit", Unit}}); }
inline Value IfcPropertyListValue(Value Name, Value Description, Value ListValues, Value Unit) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyListValue", {{"Name", Name}, {"Description", Description}, {"ListValues", ListValues}, {"Unit", Unit}}); }
inline Value IfcPropertyReferenceValue(Value Name, Value Description, Value UsageName, Value PropertyReference) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyReferenceValue", {{"Name", Name}, {"Description", Description}, {"UsageName", UsageName}, {"PropertyReference", PropertyReference}}); }
inline Value IfcPropertySet(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value HasProperties) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertySet", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"HasProperties", HasProperties}}); }
inline Value IfcPropertySetDefinition(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertySetDefinition", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcPropertySingleValue(Value Name, Value Description, Value NominalValue, Value Unit) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertySingleValue", {{"Name", Name}, {"Description", Description}, {"NominalValue", NominalValue}, {"Unit", Unit}}); }
inline Value IfcPropertyTableValue(Value Name, Value Description, Value DefiningValues, Value DefinedValues, Value Expression, Value DefiningUnit, Value DefinedUnit) { return ifcapi::express::make_entity("IFC2X3", "IfcPropertyTableValue", {{"Name", Name}, {"Description", Description}, {"DefiningValues", DefiningValues}, {"DefinedValues", DefinedValues}, {"Expression", Expression}, {"DefiningUnit", DefiningUnit}, {"DefinedUnit", DefinedUnit}}); }
inline Value IfcProtectiveDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcProtectiveDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcProxy(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value ProxyType, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcProxy", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"ProxyType", ProxyType}, {"Tag", Tag}}); }
inline Value IfcPumpType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcPumpType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcQuantityArea(Value Name, Value Description, Value Unit, Value AreaValue) { return ifcapi::express::make_entity("IFC2X3", "IfcQuantityArea", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"AreaValue", AreaValue}}); }
inline Value IfcQuantityCount(Value Name, Value Description, Value Unit, Value CountValue) { return ifcapi::express::make_entity("IFC2X3", "IfcQuantityCount", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"CountValue", CountValue}}); }
inline Value IfcQuantityLength(Value Name, Value Description, Value Unit, Value LengthValue) { return ifcapi::express::make_entity("IFC2X3", "IfcQuantityLength", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"LengthValue", LengthValue}}); }
inline Value IfcQuantityTime(Value Name, Value Description, Value Unit, Value TimeValue) { return ifcapi::express::make_entity("IFC2X3", "IfcQuantityTime", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"TimeValue", TimeValue}}); }
inline Value IfcQuantityVolume(Value Name, Value Description, Value Unit, Value VolumeValue) { return ifcapi::express::make_entity("IFC2X3", "IfcQuantityVolume", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"VolumeValue", VolumeValue}}); }
inline Value IfcQuantityWeight(Value Name, Value Description, Value Unit, Value WeightValue) { return ifcapi::express::make_entity("IFC2X3", "IfcQuantityWeight", {{"Name", Name}, {"Description", Description}, {"Unit", Unit}, {"WeightValue", WeightValue}}); }
inline Value IfcRadiusDimension(Value Contents) { return ifcapi::express::make_entity("IFC2X3", "IfcRadiusDimension", {{"Contents", Contents}}); }
inline Value IfcRailing(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcRailing", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRailingType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcRailingType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRamp(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value ShapeType) { return ifcapi::express::make_entity("IFC2X3", "IfcRamp", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"ShapeType", ShapeType}}); }
inline Value IfcRampFlight(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcRampFlight", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcRampFlightType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcRampFlightType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcRationalBezierCurve(Value Degree, Value ControlPointsList, Value CurveForm, Value ClosedCurve, Value SelfIntersect, Value WeightsData) { return ifcapi::express::make_entity("IFC2X3", "IfcRationalBezierCurve", {{"Degree", Degree}, {"ControlPointsList", ControlPointsList}, {"CurveForm", CurveForm}, {"ClosedCurve", ClosedCurve}, {"SelfIntersect", SelfIntersect}, {"WeightsData", WeightsData}}); }
inline Value IfcRectangleHollowProfileDef(Value ProfileType, Value ProfileName, Value Position, Value XDim, Value YDim, Value WallThickness, Value InnerFilletRadius, Value OuterFilletRadius) { return ifcapi::express::make_entity("IFC2X3", "IfcRectangleHollowProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"XDim", XDim}, {"YDim", YDim}, {"WallThickness", WallThickness}, {"InnerFilletRadius", InnerFilletRadius}, {"OuterFilletRadius", OuterFilletRadius}}); }
inline Value IfcRectangleProfileDef(Value ProfileType, Value ProfileName, Value Position, Value XDim, Value YDim) { return ifcapi::express::make_entity("IFC2X3", "IfcRectangleProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"XDim", XDim}, {"YDim", YDim}}); }
inline Value IfcRectangularPyramid(Value Position, Value XLength, Value YLength, Value Height) { return ifcapi::express::make_entity("IFC2X3", "IfcRectangularPyramid", {{"Position", Position}, {"XLength", XLength}, {"YLength", YLength}, {"Height", Height}}); }
inline Value IfcRectangularTrimmedSurface(Value BasisSurface, Value U1, Value V1, Value U2, Value V2, Value Usense, Value Vsense) { return ifcapi::express::make_entity("IFC2X3", "IfcRectangularTrimmedSurface", {{"BasisSurface", BasisSurface}, {"U1", U1}, {"V1", V1}, {"U2", U2}, {"V2", V2}, {"Usense", Usense}, {"Vsense", Vsense}}); }
inline Value IfcReferencesValueDocument(Value ReferencedDocument, Value ReferencingValues, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcReferencesValueDocument", {{"ReferencedDocument", ReferencedDocument}, {"ReferencingValues", ReferencingValues}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRegularTimeSeries(Value Name, Value Description, Value StartTime, Value EndTime, Value TimeSeriesDataType, Value DataOrigin, Value UserDefinedDataOrigin, Value Unit, Value TimeStep, Value Values) { return ifcapi::express::make_entity("IFC2X3", "IfcRegularTimeSeries", {{"Name", Name}, {"Description", Description}, {"StartTime", StartTime}, {"EndTime", EndTime}, {"TimeSeriesDataType", TimeSeriesDataType}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"Unit", Unit}, {"TimeStep", TimeStep}, {"Values", Values}}); }
inline Value IfcReinforcementBarProperties(Value TotalCrossSectionArea, Value SteelGrade, Value BarSurface, Value EffectiveDepth, Value NominalBarDiameter, Value BarCount) { return ifcapi::express::make_entity("IFC2X3", "IfcReinforcementBarProperties", {{"TotalCrossSectionArea", TotalCrossSectionArea}, {"SteelGrade", SteelGrade}, {"BarSurface", BarSurface}, {"EffectiveDepth", EffectiveDepth}, {"NominalBarDiameter", NominalBarDiameter}, {"BarCount", BarCount}}); }
inline Value IfcReinforcementDefinitionProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value DefinitionType, Value ReinforcementSectionDefinitions) { return ifcapi::express::make_entity("IFC2X3", "IfcReinforcementDefinitionProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"DefinitionType", DefinitionType}, {"ReinforcementSectionDefinitions", ReinforcementSectionDefinitions}}); }
inline Value IfcReinforcingBar(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade, Value NominalDiameter, Value CrossSectionArea, Value BarLength, Value BarRole, Value BarSurface) { return ifcapi::express::make_entity("IFC2X3", "IfcReinforcingBar", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}, {"NominalDiameter", NominalDiameter}, {"CrossSectionArea", CrossSectionArea}, {"BarLength", BarLength}, {"BarRole", BarRole}, {"BarSurface", BarSurface}}); }
inline Value IfcReinforcingElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade) { return ifcapi::express::make_entity("IFC2X3", "IfcReinforcingElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}}); }
inline Value IfcReinforcingMesh(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade, Value MeshLength, Value MeshWidth, Value LongitudinalBarNominalDiameter, Value TransverseBarNominalDiameter, Value LongitudinalBarCrossSectionArea, Value TransverseBarCrossSectionArea, Value LongitudinalBarSpacing, Value TransverseBarSpacing) { return ifcapi::express::make_entity("IFC2X3", "IfcReinforcingMesh", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}, {"MeshLength", MeshLength}, {"MeshWidth", MeshWidth}, {"LongitudinalBarNominalDiameter", LongitudinalBarNominalDiameter}, {"TransverseBarNominalDiameter", TransverseBarNominalDiameter}, {"LongitudinalBarCrossSectionArea", LongitudinalBarCrossSectionArea}, {"TransverseBarCrossSectionArea", TransverseBarCrossSectionArea}, {"LongitudinalBarSpacing", LongitudinalBarSpacing}, {"TransverseBarSpacing", TransverseBarSpacing}}); }
inline Value IfcRelAggregates(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingObject, Value RelatedObjects) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAggregates", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingObject", RelatingObject}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelAssigns(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssigns", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}}); }
inline Value IfcRelAssignsTasks(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingControl, Value TimeForTask) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsTasks", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingControl", RelatingControl}, {"TimeForTask", TimeForTask}}); }
inline Value IfcRelAssignsToActor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingActor, Value ActingRole) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsToActor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingActor", RelatingActor}, {"ActingRole", ActingRole}}); }
inline Value IfcRelAssignsToControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingControl) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsToControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingControl", RelatingControl}}); }
inline Value IfcRelAssignsToGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingGroup) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsToGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingGroup", RelatingGroup}}); }
inline Value IfcRelAssignsToProcess(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingProcess, Value QuantityInProcess) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsToProcess", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingProcess", RelatingProcess}, {"QuantityInProcess", QuantityInProcess}}); }
inline Value IfcRelAssignsToProduct(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingProduct) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsToProduct", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingProduct", RelatingProduct}}); }
inline Value IfcRelAssignsToProjectOrder(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingControl) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsToProjectOrder", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingControl", RelatingControl}}); }
inline Value IfcRelAssignsToResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingResource) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssignsToResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingResource", RelatingResource}}); }
inline Value IfcRelAssociates(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociates", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelAssociatesAppliedValue(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingAppliedValue) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesAppliedValue", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingAppliedValue", RelatingAppliedValue}}); }
inline Value IfcRelAssociatesApproval(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingApproval) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesApproval", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingApproval", RelatingApproval}}); }
inline Value IfcRelAssociatesClassification(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingClassification) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesClassification", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingClassification", RelatingClassification}}); }
inline Value IfcRelAssociatesConstraint(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value Intent, Value RelatingConstraint) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesConstraint", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"Intent", Intent}, {"RelatingConstraint", RelatingConstraint}}); }
inline Value IfcRelAssociatesDocument(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingDocument) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesDocument", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingDocument", RelatingDocument}}); }
inline Value IfcRelAssociatesLibrary(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingLibrary) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesLibrary", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingLibrary", RelatingLibrary}}); }
inline Value IfcRelAssociatesMaterial(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingMaterial) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesMaterial", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingMaterial", RelatingMaterial}}); }
inline Value IfcRelAssociatesProfileProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingProfileProperties, Value ProfileSectionLocation, Value ProfileOrientation) { return ifcapi::express::make_entity("IFC2X3", "IfcRelAssociatesProfileProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingProfileProperties", RelatingProfileProperties}, {"ProfileSectionLocation", ProfileSectionLocation}, {"ProfileOrientation", ProfileOrientation}}); }
inline Value IfcRelConnects(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnects", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRelConnectsElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ConnectionGeometry, Value RelatingElement, Value RelatedElement) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ConnectionGeometry", ConnectionGeometry}, {"RelatingElement", RelatingElement}, {"RelatedElement", RelatedElement}}); }
inline Value IfcRelConnectsPathElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ConnectionGeometry, Value RelatingElement, Value RelatedElement, Value RelatingPriorities, Value RelatedPriorities, Value RelatedConnectionType, Value RelatingConnectionType) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsPathElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ConnectionGeometry", ConnectionGeometry}, {"RelatingElement", RelatingElement}, {"RelatedElement", RelatedElement}, {"RelatingPriorities", RelatingPriorities}, {"RelatedPriorities", RelatedPriorities}, {"RelatedConnectionType", RelatedConnectionType}, {"RelatingConnectionType", RelatingConnectionType}}); }
inline Value IfcRelConnectsPortToElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingPort, Value RelatedElement) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsPortToElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingPort", RelatingPort}, {"RelatedElement", RelatedElement}}); }
inline Value IfcRelConnectsPorts(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingPort, Value RelatedPort, Value RealizingElement) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsPorts", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingPort", RelatingPort}, {"RelatedPort", RelatedPort}, {"RealizingElement", RealizingElement}}); }
inline Value IfcRelConnectsStructuralActivity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingElement, Value RelatedStructuralActivity) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsStructuralActivity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingElement", RelatingElement}, {"RelatedStructuralActivity", RelatedStructuralActivity}}); }
inline Value IfcRelConnectsStructuralElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingElement, Value RelatedStructuralMember) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsStructuralElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingElement", RelatingElement}, {"RelatedStructuralMember", RelatedStructuralMember}}); }
inline Value IfcRelConnectsStructuralMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingStructuralMember, Value RelatedStructuralConnection, Value AppliedCondition, Value AdditionalConditions, Value SupportedLength, Value ConditionCoordinateSystem) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsStructuralMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingStructuralMember", RelatingStructuralMember}, {"RelatedStructuralConnection", RelatedStructuralConnection}, {"AppliedCondition", AppliedCondition}, {"AdditionalConditions", AdditionalConditions}, {"SupportedLength", SupportedLength}, {"ConditionCoordinateSystem", ConditionCoordinateSystem}}); }
inline Value IfcRelConnectsWithEccentricity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingStructuralMember, Value RelatedStructuralConnection, Value AppliedCondition, Value AdditionalConditions, Value SupportedLength, Value ConditionCoordinateSystem, Value ConnectionConstraint) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsWithEccentricity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingStructuralMember", RelatingStructuralMember}, {"RelatedStructuralConnection", RelatedStructuralConnection}, {"AppliedCondition", AppliedCondition}, {"AdditionalConditions", AdditionalConditions}, {"SupportedLength", SupportedLength}, {"ConditionCoordinateSystem", ConditionCoordinateSystem}, {"ConnectionConstraint", ConnectionConstraint}}); }
inline Value IfcRelConnectsWithRealizingElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ConnectionGeometry, Value RelatingElement, Value RelatedElement, Value RealizingElements, Value ConnectionType) { return ifcapi::express::make_entity("IFC2X3", "IfcRelConnectsWithRealizingElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ConnectionGeometry", ConnectionGeometry}, {"RelatingElement", RelatingElement}, {"RelatedElement", RelatedElement}, {"RealizingElements", RealizingElements}, {"ConnectionType", ConnectionType}}); }
inline Value IfcRelContainedInSpatialStructure(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedElements, Value RelatingStructure) { return ifcapi::express::make_entity("IFC2X3", "IfcRelContainedInSpatialStructure", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedElements", RelatedElements}, {"RelatingStructure", RelatingStructure}}); }
inline Value IfcRelCoversBldgElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingBuildingElement, Value RelatedCoverings) { return ifcapi::express::make_entity("IFC2X3", "IfcRelCoversBldgElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingBuildingElement", RelatingBuildingElement}, {"RelatedCoverings", RelatedCoverings}}); }
inline Value IfcRelCoversSpaces(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedSpace, Value RelatedCoverings) { return ifcapi::express::make_entity("IFC2X3", "IfcRelCoversSpaces", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedSpace", RelatedSpace}, {"RelatedCoverings", RelatedCoverings}}); }
inline Value IfcRelDecomposes(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingObject, Value RelatedObjects) { return ifcapi::express::make_entity("IFC2X3", "IfcRelDecomposes", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingObject", RelatingObject}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelDefines(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects) { return ifcapi::express::make_entity("IFC2X3", "IfcRelDefines", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelDefinesByProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingPropertyDefinition) { return ifcapi::express::make_entity("IFC2X3", "IfcRelDefinesByProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingPropertyDefinition", RelatingPropertyDefinition}}); }
inline Value IfcRelDefinesByType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingType) { return ifcapi::express::make_entity("IFC2X3", "IfcRelDefinesByType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingType", RelatingType}}); }
inline Value IfcRelFillsElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingOpeningElement, Value RelatedBuildingElement) { return ifcapi::express::make_entity("IFC2X3", "IfcRelFillsElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingOpeningElement", RelatingOpeningElement}, {"RelatedBuildingElement", RelatedBuildingElement}}); }
inline Value IfcRelFlowControlElements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedControlElements, Value RelatingFlowElement) { return ifcapi::express::make_entity("IFC2X3", "IfcRelFlowControlElements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedControlElements", RelatedControlElements}, {"RelatingFlowElement", RelatingFlowElement}}); }
inline Value IfcRelInteractionRequirements(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value DailyInteraction, Value ImportanceRating, Value LocationOfInteraction, Value RelatedSpaceProgram, Value RelatingSpaceProgram) { return ifcapi::express::make_entity("IFC2X3", "IfcRelInteractionRequirements", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"DailyInteraction", DailyInteraction}, {"ImportanceRating", ImportanceRating}, {"LocationOfInteraction", LocationOfInteraction}, {"RelatedSpaceProgram", RelatedSpaceProgram}, {"RelatingSpaceProgram", RelatingSpaceProgram}}); }
inline Value IfcRelNests(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingObject, Value RelatedObjects) { return ifcapi::express::make_entity("IFC2X3", "IfcRelNests", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingObject", RelatingObject}, {"RelatedObjects", RelatedObjects}}); }
inline Value IfcRelOccupiesSpaces(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingActor, Value ActingRole) { return ifcapi::express::make_entity("IFC2X3", "IfcRelOccupiesSpaces", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingActor", RelatingActor}, {"ActingRole", ActingRole}}); }
inline Value IfcRelOverridesProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatingPropertyDefinition, Value OverridingProperties) { return ifcapi::express::make_entity("IFC2X3", "IfcRelOverridesProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatingPropertyDefinition", RelatingPropertyDefinition}, {"OverridingProperties", OverridingProperties}}); }
inline Value IfcRelProjectsElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingElement, Value RelatedFeatureElement) { return ifcapi::express::make_entity("IFC2X3", "IfcRelProjectsElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingElement", RelatingElement}, {"RelatedFeatureElement", RelatedFeatureElement}}); }
inline Value IfcRelReferencedInSpatialStructure(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedElements, Value RelatingStructure) { return ifcapi::express::make_entity("IFC2X3", "IfcRelReferencedInSpatialStructure", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedElements", RelatedElements}, {"RelatingStructure", RelatingStructure}}); }
inline Value IfcRelSchedulesCostItems(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatedObjects, Value RelatedObjectsType, Value RelatingControl) { return ifcapi::express::make_entity("IFC2X3", "IfcRelSchedulesCostItems", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatedObjects", RelatedObjects}, {"RelatedObjectsType", RelatedObjectsType}, {"RelatingControl", RelatingControl}}); }
inline Value IfcRelSequence(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingProcess, Value RelatedProcess, Value TimeLag, Value SequenceType) { return ifcapi::express::make_entity("IFC2X3", "IfcRelSequence", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingProcess", RelatingProcess}, {"RelatedProcess", RelatedProcess}, {"TimeLag", TimeLag}, {"SequenceType", SequenceType}}); }
inline Value IfcRelServicesBuildings(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingSystem, Value RelatedBuildings) { return ifcapi::express::make_entity("IFC2X3", "IfcRelServicesBuildings", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingSystem", RelatingSystem}, {"RelatedBuildings", RelatedBuildings}}); }
inline Value IfcRelSpaceBoundary(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingSpace, Value RelatedBuildingElement, Value ConnectionGeometry, Value PhysicalOrVirtualBoundary, Value InternalOrExternalBoundary) { return ifcapi::express::make_entity("IFC2X3", "IfcRelSpaceBoundary", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingSpace", RelatingSpace}, {"RelatedBuildingElement", RelatedBuildingElement}, {"ConnectionGeometry", ConnectionGeometry}, {"PhysicalOrVirtualBoundary", PhysicalOrVirtualBoundary}, {"InternalOrExternalBoundary", InternalOrExternalBoundary}}); }
inline Value IfcRelVoidsElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value RelatingBuildingElement, Value RelatedOpeningElement) { return ifcapi::express::make_entity("IFC2X3", "IfcRelVoidsElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"RelatingBuildingElement", RelatingBuildingElement}, {"RelatedOpeningElement", RelatedOpeningElement}}); }
inline Value IfcRelationship(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcRelationship", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRelaxation(Value RelaxationValue, Value InitialStress) { return ifcapi::express::make_entity("IFC2X3", "IfcRelaxation", {{"RelaxationValue", RelaxationValue}, {"InitialStress", InitialStress}}); }
inline Value IfcRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC2X3", "IfcRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcRepresentationContext(Value ContextIdentifier, Value ContextType) { return ifcapi::express::make_entity("IFC2X3", "IfcRepresentationContext", {{"ContextIdentifier", ContextIdentifier}, {"ContextType", ContextType}}); }
inline Value IfcRepresentationItem() { return ifcapi::express::make_entity("IFC2X3", "IfcRepresentationItem", {}); }
inline Value IfcRepresentationMap(Value MappingOrigin, Value MappedRepresentation) { return ifcapi::express::make_entity("IFC2X3", "IfcRepresentationMap", {{"MappingOrigin", MappingOrigin}, {"MappedRepresentation", MappedRepresentation}}); }
inline Value IfcResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcRevolvedAreaSolid(Value SweptArea, Value Position, Value Axis, Value Angle) { return ifcapi::express::make_entity("IFC2X3", "IfcRevolvedAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}, {"Axis", Axis}, {"Angle", Angle}}); }
inline Value IfcRibPlateProfileProperties(Value ProfileName, Value ProfileDefinition, Value Thickness, Value RibHeight, Value RibWidth, Value RibSpacing, Value Direction) { return ifcapi::express::make_entity("IFC2X3", "IfcRibPlateProfileProperties", {{"ProfileName", ProfileName}, {"ProfileDefinition", ProfileDefinition}, {"Thickness", Thickness}, {"RibHeight", RibHeight}, {"RibWidth", RibWidth}, {"RibSpacing", RibSpacing}, {"Direction", Direction}}); }
inline Value IfcRightCircularCone(Value Position, Value Height, Value BottomRadius) { return ifcapi::express::make_entity("IFC2X3", "IfcRightCircularCone", {{"Position", Position}, {"Height", Height}, {"BottomRadius", BottomRadius}}); }
inline Value IfcRightCircularCylinder(Value Position, Value Height, Value Radius) { return ifcapi::express::make_entity("IFC2X3", "IfcRightCircularCylinder", {{"Position", Position}, {"Height", Height}, {"Radius", Radius}}); }
inline Value IfcRoof(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value ShapeType) { return ifcapi::express::make_entity("IFC2X3", "IfcRoof", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"ShapeType", ShapeType}}); }
inline Value IfcRoot(Value GlobalId, Value OwnerHistory, Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcRoot", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}}); }
inline Value IfcRoundedEdgeFeature(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value FeatureLength, Value Radius) { return ifcapi::express::make_entity("IFC2X3", "IfcRoundedEdgeFeature", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"FeatureLength", FeatureLength}, {"Radius", Radius}}); }
inline Value IfcRoundedRectangleProfileDef(Value ProfileType, Value ProfileName, Value Position, Value XDim, Value YDim, Value RoundingRadius) { return ifcapi::express::make_entity("IFC2X3", "IfcRoundedRectangleProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"XDim", XDim}, {"YDim", YDim}, {"RoundingRadius", RoundingRadius}}); }
inline Value IfcSIUnit(Value Dimensions, Value UnitType, Value Prefix, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcSIUnit", {{"Dimensions", Dimensions}, {"UnitType", UnitType}, {"Prefix", Prefix}, {"Name", Name}}); }
inline Value IfcSanitaryTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcSanitaryTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcScheduleTimeControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ActualStart, Value EarlyStart, Value LateStart, Value ScheduleStart, Value ActualFinish, Value EarlyFinish, Value LateFinish, Value ScheduleFinish, Value ScheduleDuration, Value ActualDuration, Value RemainingTime, Value FreeFloat, Value TotalFloat, Value IsCritical, Value StatusTime, Value StartFloat, Value FinishFloat, Value Completion) { return ifcapi::express::make_entity("IFC2X3", "IfcScheduleTimeControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ActualStart", ActualStart}, {"EarlyStart", EarlyStart}, {"LateStart", LateStart}, {"ScheduleStart", ScheduleStart}, {"ActualFinish", ActualFinish}, {"EarlyFinish", EarlyFinish}, {"LateFinish", LateFinish}, {"ScheduleFinish", ScheduleFinish}, {"ScheduleDuration", ScheduleDuration}, {"ActualDuration", ActualDuration}, {"RemainingTime", RemainingTime}, {"FreeFloat", FreeFloat}, {"TotalFloat", TotalFloat}, {"IsCritical", IsCritical}, {"StatusTime", StatusTime}, {"StartFloat", StartFloat}, {"FinishFloat", FinishFloat}, {"Completion", Completion}}); }
inline Value IfcSectionProperties(Value SectionType, Value StartProfile, Value EndProfile) { return ifcapi::express::make_entity("IFC2X3", "IfcSectionProperties", {{"SectionType", SectionType}, {"StartProfile", StartProfile}, {"EndProfile", EndProfile}}); }
inline Value IfcSectionReinforcementProperties(Value LongitudinalStartPosition, Value LongitudinalEndPosition, Value TransversePosition, Value ReinforcementRole, Value SectionDefinition, Value CrossSectionReinforcementDefinitions) { return ifcapi::express::make_entity("IFC2X3", "IfcSectionReinforcementProperties", {{"LongitudinalStartPosition", LongitudinalStartPosition}, {"LongitudinalEndPosition", LongitudinalEndPosition}, {"TransversePosition", TransversePosition}, {"ReinforcementRole", ReinforcementRole}, {"SectionDefinition", SectionDefinition}, {"CrossSectionReinforcementDefinitions", CrossSectionReinforcementDefinitions}}); }
inline Value IfcSectionedSpine(Value SpineCurve, Value CrossSections, Value CrossSectionPositions) { return ifcapi::express::make_entity("IFC2X3", "IfcSectionedSpine", {{"SpineCurve", SpineCurve}, {"CrossSections", CrossSections}, {"CrossSectionPositions", CrossSectionPositions}}); }
inline Value IfcSensorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcSensorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcServiceLife(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ServiceLifeType, Value ServiceLifeDuration) { return ifcapi::express::make_entity("IFC2X3", "IfcServiceLife", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ServiceLifeType", ServiceLifeType}, {"ServiceLifeDuration", ServiceLifeDuration}}); }
inline Value IfcServiceLifeFactor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value PredefinedType, Value UpperValue, Value MostUsedValue, Value LowerValue) { return ifcapi::express::make_entity("IFC2X3", "IfcServiceLifeFactor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"PredefinedType", PredefinedType}, {"UpperValue", UpperValue}, {"MostUsedValue", MostUsedValue}, {"LowerValue", LowerValue}}); }
inline Value IfcShapeAspect(Value ShapeRepresentations, Value Name, Value Description, Value ProductDefinitional, Value PartOfProductDefinitionShape) { return ifcapi::express::make_entity("IFC2X3", "IfcShapeAspect", {{"ShapeRepresentations", ShapeRepresentations}, {"Name", Name}, {"Description", Description}, {"ProductDefinitional", ProductDefinitional}, {"PartOfProductDefinitionShape", PartOfProductDefinitionShape}}); }
inline Value IfcShapeModel(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC2X3", "IfcShapeModel", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcShapeRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC2X3", "IfcShapeRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcShellBasedSurfaceModel(Value SbsmBoundary) { return ifcapi::express::make_entity("IFC2X3", "IfcShellBasedSurfaceModel", {{"SbsmBoundary", SbsmBoundary}}); }
inline Value IfcSimpleProperty(Value Name, Value Description) { return ifcapi::express::make_entity("IFC2X3", "IfcSimpleProperty", {{"Name", Name}, {"Description", Description}}); }
inline Value IfcSite(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value RefLatitude, Value RefLongitude, Value RefElevation, Value LandTitleNumber, Value SiteAddress) { return ifcapi::express::make_entity("IFC2X3", "IfcSite", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"RefLatitude", RefLatitude}, {"RefLongitude", RefLongitude}, {"RefElevation", RefElevation}, {"LandTitleNumber", LandTitleNumber}, {"SiteAddress", SiteAddress}}); }
inline Value IfcSlab(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcSlab", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSlabType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcSlabType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSlippageConnectionCondition(Value Name, Value SlippageX, Value SlippageY, Value SlippageZ) { return ifcapi::express::make_entity("IFC2X3", "IfcSlippageConnectionCondition", {{"Name", Name}, {"SlippageX", SlippageX}, {"SlippageY", SlippageY}, {"SlippageZ", SlippageZ}}); }
inline Value IfcSolidModel() { return ifcapi::express::make_entity("IFC2X3", "IfcSolidModel", {}); }
inline Value IfcSoundProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value IsAttenuating, Value SoundScale, Value SoundValues) { return ifcapi::express::make_entity("IFC2X3", "IfcSoundProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"IsAttenuating", IsAttenuating}, {"SoundScale", SoundScale}, {"SoundValues", SoundValues}}); }
inline Value IfcSoundValue(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value SoundLevelTimeSeries, Value Frequency, Value SoundLevelSingleValue) { return ifcapi::express::make_entity("IFC2X3", "IfcSoundValue", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"SoundLevelTimeSeries", SoundLevelTimeSeries}, {"Frequency", Frequency}, {"SoundLevelSingleValue", SoundLevelSingleValue}}); }
inline Value IfcSpace(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType, Value InteriorOrExteriorSpace, Value ElevationWithFlooring) { return ifcapi::express::make_entity("IFC2X3", "IfcSpace", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}, {"InteriorOrExteriorSpace", InteriorOrExteriorSpace}, {"ElevationWithFlooring", ElevationWithFlooring}}); }
inline Value IfcSpaceHeaterType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcSpaceHeaterType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSpaceProgram(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value SpaceProgramIdentifier, Value MaxRequiredArea, Value MinRequiredArea, Value RequestedLocation, Value StandardRequiredArea) { return ifcapi::express::make_entity("IFC2X3", "IfcSpaceProgram", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"SpaceProgramIdentifier", SpaceProgramIdentifier}, {"MaxRequiredArea", MaxRequiredArea}, {"MinRequiredArea", MinRequiredArea}, {"RequestedLocation", RequestedLocation}, {"StandardRequiredArea", StandardRequiredArea}}); }
inline Value IfcSpaceThermalLoadProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableValueRatio, Value ThermalLoadSource, Value PropertySource, Value SourceDescription, Value MaximumValue, Value MinimumValue, Value ThermalLoadTimeSeriesValues, Value UserDefinedThermalLoadSource, Value UserDefinedPropertySource, Value ThermalLoadType) { return ifcapi::express::make_entity("IFC2X3", "IfcSpaceThermalLoadProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableValueRatio", ApplicableValueRatio}, {"ThermalLoadSource", ThermalLoadSource}, {"PropertySource", PropertySource}, {"SourceDescription", SourceDescription}, {"MaximumValue", MaximumValue}, {"MinimumValue", MinimumValue}, {"ThermalLoadTimeSeriesValues", ThermalLoadTimeSeriesValues}, {"UserDefinedThermalLoadSource", UserDefinedThermalLoadSource}, {"UserDefinedPropertySource", UserDefinedPropertySource}, {"ThermalLoadType", ThermalLoadType}}); }
inline Value IfcSpaceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcSpaceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSpatialStructureElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value LongName, Value CompositionType) { return ifcapi::express::make_entity("IFC2X3", "IfcSpatialStructureElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"LongName", LongName}, {"CompositionType", CompositionType}}); }
inline Value IfcSpatialStructureElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcSpatialStructureElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcSphere(Value Position, Value Radius) { return ifcapi::express::make_entity("IFC2X3", "IfcSphere", {{"Position", Position}, {"Radius", Radius}}); }
inline Value IfcStackTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcStackTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStair(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value ShapeType) { return ifcapi::express::make_entity("IFC2X3", "IfcStair", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"ShapeType", ShapeType}}); }
inline Value IfcStairFlight(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value NumberOfRiser, Value NumberOfTreads, Value RiserHeight, Value TreadLength) { return ifcapi::express::make_entity("IFC2X3", "IfcStairFlight", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"NumberOfRiser", NumberOfRiser}, {"NumberOfTreads", NumberOfTreads}, {"RiserHeight", RiserHeight}, {"TreadLength", TreadLength}}); }
inline Value IfcStairFlightType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcStairFlightType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value CausedBy) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"CausedBy", CausedBy}}); }
inline Value IfcStructuralActivity(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralActivity", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}}); }
inline Value IfcStructuralAnalysisModel(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PredefinedType, Value OrientationOf2DPlane, Value LoadedBy, Value HasResults) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralAnalysisModel", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PredefinedType", PredefinedType}, {"OrientationOf2DPlane", OrientationOf2DPlane}, {"LoadedBy", LoadedBy}, {"HasResults", HasResults}}); }
inline Value IfcStructuralConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}}); }
inline Value IfcStructuralConnectionCondition(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralConnectionCondition", {{"Name", Name}}); }
inline Value IfcStructuralCurveConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralCurveConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}}); }
inline Value IfcStructuralCurveMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralCurveMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralCurveMemberVarying(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralCurveMemberVarying", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}}); }
inline Value IfcStructuralItem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralItem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcStructuralLinearAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value CausedBy, Value ProjectedOrTrue) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLinearAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"CausedBy", CausedBy}, {"ProjectedOrTrue", ProjectedOrTrue}}); }
inline Value IfcStructuralLinearActionVarying(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value CausedBy, Value ProjectedOrTrue, Value VaryingAppliedLoadLocation, Value SubsequentAppliedLoads) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLinearActionVarying", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"CausedBy", CausedBy}, {"ProjectedOrTrue", ProjectedOrTrue}, {"VaryingAppliedLoadLocation", VaryingAppliedLoadLocation}, {"SubsequentAppliedLoads", SubsequentAppliedLoads}}); }
inline Value IfcStructuralLoad(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoad", {{"Name", Name}}); }
inline Value IfcStructuralLoadGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value PredefinedType, Value ActionType, Value ActionSource, Value Coefficient, Value Purpose) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"PredefinedType", PredefinedType}, {"ActionType", ActionType}, {"ActionSource", ActionSource}, {"Coefficient", Coefficient}, {"Purpose", Purpose}}); }
inline Value IfcStructuralLoadLinearForce(Value Name, Value LinearForceX, Value LinearForceY, Value LinearForceZ, Value LinearMomentX, Value LinearMomentY, Value LinearMomentZ) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadLinearForce", {{"Name", Name}, {"LinearForceX", LinearForceX}, {"LinearForceY", LinearForceY}, {"LinearForceZ", LinearForceZ}, {"LinearMomentX", LinearMomentX}, {"LinearMomentY", LinearMomentY}, {"LinearMomentZ", LinearMomentZ}}); }
inline Value IfcStructuralLoadPlanarForce(Value Name, Value PlanarForceX, Value PlanarForceY, Value PlanarForceZ) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadPlanarForce", {{"Name", Name}, {"PlanarForceX", PlanarForceX}, {"PlanarForceY", PlanarForceY}, {"PlanarForceZ", PlanarForceZ}}); }
inline Value IfcStructuralLoadSingleDisplacement(Value Name, Value DisplacementX, Value DisplacementY, Value DisplacementZ, Value RotationalDisplacementRX, Value RotationalDisplacementRY, Value RotationalDisplacementRZ) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadSingleDisplacement", {{"Name", Name}, {"DisplacementX", DisplacementX}, {"DisplacementY", DisplacementY}, {"DisplacementZ", DisplacementZ}, {"RotationalDisplacementRX", RotationalDisplacementRX}, {"RotationalDisplacementRY", RotationalDisplacementRY}, {"RotationalDisplacementRZ", RotationalDisplacementRZ}}); }
inline Value IfcStructuralLoadSingleDisplacementDistortion(Value Name, Value DisplacementX, Value DisplacementY, Value DisplacementZ, Value RotationalDisplacementRX, Value RotationalDisplacementRY, Value RotationalDisplacementRZ, Value Distortion) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadSingleDisplacementDistortion", {{"Name", Name}, {"DisplacementX", DisplacementX}, {"DisplacementY", DisplacementY}, {"DisplacementZ", DisplacementZ}, {"RotationalDisplacementRX", RotationalDisplacementRX}, {"RotationalDisplacementRY", RotationalDisplacementRY}, {"RotationalDisplacementRZ", RotationalDisplacementRZ}, {"Distortion", Distortion}}); }
inline Value IfcStructuralLoadSingleForce(Value Name, Value ForceX, Value ForceY, Value ForceZ, Value MomentX, Value MomentY, Value MomentZ) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadSingleForce", {{"Name", Name}, {"ForceX", ForceX}, {"ForceY", ForceY}, {"ForceZ", ForceZ}, {"MomentX", MomentX}, {"MomentY", MomentY}, {"MomentZ", MomentZ}}); }
inline Value IfcStructuralLoadSingleForceWarping(Value Name, Value ForceX, Value ForceY, Value ForceZ, Value MomentX, Value MomentY, Value MomentZ, Value WarpingMoment) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadSingleForceWarping", {{"Name", Name}, {"ForceX", ForceX}, {"ForceY", ForceY}, {"ForceZ", ForceZ}, {"MomentX", MomentX}, {"MomentY", MomentY}, {"MomentZ", MomentZ}, {"WarpingMoment", WarpingMoment}}); }
inline Value IfcStructuralLoadStatic(Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadStatic", {{"Name", Name}}); }
inline Value IfcStructuralLoadTemperature(Value Name, Value DeltaT_Constant, Value DeltaT_Y, Value DeltaT_Z) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralLoadTemperature", {{"Name", Name}, {"DeltaT_Constant", DeltaT_Constant}, {"DeltaT_Y", DeltaT_Y}, {"DeltaT_Z", DeltaT_Z}}); }
inline Value IfcStructuralMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}}); }
inline Value IfcStructuralPlanarAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value CausedBy, Value ProjectedOrTrue) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralPlanarAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"CausedBy", CausedBy}, {"ProjectedOrTrue", ProjectedOrTrue}}); }
inline Value IfcStructuralPlanarActionVarying(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value CausedBy, Value ProjectedOrTrue, Value VaryingAppliedLoadLocation, Value SubsequentAppliedLoads) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralPlanarActionVarying", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"CausedBy", CausedBy}, {"ProjectedOrTrue", ProjectedOrTrue}, {"VaryingAppliedLoadLocation", VaryingAppliedLoadLocation}, {"SubsequentAppliedLoads", SubsequentAppliedLoads}}); }
inline Value IfcStructuralPointAction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal, Value DestabilizingLoad, Value CausedBy) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralPointAction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}, {"DestabilizingLoad", DestabilizingLoad}, {"CausedBy", CausedBy}}); }
inline Value IfcStructuralPointConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralPointConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}}); }
inline Value IfcStructuralPointReaction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralPointReaction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}}); }
inline Value IfcStructuralProfileProperties(Value ProfileName, Value ProfileDefinition, Value PhysicalWeight, Value Perimeter, Value MinimumPlateThickness, Value MaximumPlateThickness, Value CrossSectionArea, Value TorsionalConstantX, Value MomentOfInertiaYZ, Value MomentOfInertiaY, Value MomentOfInertiaZ, Value WarpingConstant, Value ShearCentreZ, Value ShearCentreY, Value ShearDeformationAreaZ, Value ShearDeformationAreaY, Value MaximumSectionModulusY, Value MinimumSectionModulusY, Value MaximumSectionModulusZ, Value MinimumSectionModulusZ, Value TorsionalSectionModulus, Value CentreOfGravityInX, Value CentreOfGravityInY) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralProfileProperties", {{"ProfileName", ProfileName}, {"ProfileDefinition", ProfileDefinition}, {"PhysicalWeight", PhysicalWeight}, {"Perimeter", Perimeter}, {"MinimumPlateThickness", MinimumPlateThickness}, {"MaximumPlateThickness", MaximumPlateThickness}, {"CrossSectionArea", CrossSectionArea}, {"TorsionalConstantX", TorsionalConstantX}, {"MomentOfInertiaYZ", MomentOfInertiaYZ}, {"MomentOfInertiaY", MomentOfInertiaY}, {"MomentOfInertiaZ", MomentOfInertiaZ}, {"WarpingConstant", WarpingConstant}, {"ShearCentreZ", ShearCentreZ}, {"ShearCentreY", ShearCentreY}, {"ShearDeformationAreaZ", ShearDeformationAreaZ}, {"ShearDeformationAreaY", ShearDeformationAreaY}, {"MaximumSectionModulusY", MaximumSectionModulusY}, {"MinimumSectionModulusY", MinimumSectionModulusY}, {"MaximumSectionModulusZ", MaximumSectionModulusZ}, {"MinimumSectionModulusZ", MinimumSectionModulusZ}, {"TorsionalSectionModulus", TorsionalSectionModulus}, {"CentreOfGravityInX", CentreOfGravityInX}, {"CentreOfGravityInY", CentreOfGravityInY}}); }
inline Value IfcStructuralReaction(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedLoad, Value GlobalOrLocal) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralReaction", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedLoad", AppliedLoad}, {"GlobalOrLocal", GlobalOrLocal}}); }
inline Value IfcStructuralResultGroup(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TheoryType, Value ResultForLoadGroup, Value IsLinear) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralResultGroup", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TheoryType", TheoryType}, {"ResultForLoadGroup", ResultForLoadGroup}, {"IsLinear", IsLinear}}); }
inline Value IfcStructuralSteelProfileProperties(Value ProfileName, Value ProfileDefinition, Value PhysicalWeight, Value Perimeter, Value MinimumPlateThickness, Value MaximumPlateThickness, Value CrossSectionArea, Value TorsionalConstantX, Value MomentOfInertiaYZ, Value MomentOfInertiaY, Value MomentOfInertiaZ, Value WarpingConstant, Value ShearCentreZ, Value ShearCentreY, Value ShearDeformationAreaZ, Value ShearDeformationAreaY, Value MaximumSectionModulusY, Value MinimumSectionModulusY, Value MaximumSectionModulusZ, Value MinimumSectionModulusZ, Value TorsionalSectionModulus, Value CentreOfGravityInX, Value CentreOfGravityInY, Value ShearAreaZ, Value ShearAreaY, Value PlasticShapeFactorY, Value PlasticShapeFactorZ) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralSteelProfileProperties", {{"ProfileName", ProfileName}, {"ProfileDefinition", ProfileDefinition}, {"PhysicalWeight", PhysicalWeight}, {"Perimeter", Perimeter}, {"MinimumPlateThickness", MinimumPlateThickness}, {"MaximumPlateThickness", MaximumPlateThickness}, {"CrossSectionArea", CrossSectionArea}, {"TorsionalConstantX", TorsionalConstantX}, {"MomentOfInertiaYZ", MomentOfInertiaYZ}, {"MomentOfInertiaY", MomentOfInertiaY}, {"MomentOfInertiaZ", MomentOfInertiaZ}, {"WarpingConstant", WarpingConstant}, {"ShearCentreZ", ShearCentreZ}, {"ShearCentreY", ShearCentreY}, {"ShearDeformationAreaZ", ShearDeformationAreaZ}, {"ShearDeformationAreaY", ShearDeformationAreaY}, {"MaximumSectionModulusY", MaximumSectionModulusY}, {"MinimumSectionModulusY", MinimumSectionModulusY}, {"MaximumSectionModulusZ", MaximumSectionModulusZ}, {"MinimumSectionModulusZ", MinimumSectionModulusZ}, {"TorsionalSectionModulus", TorsionalSectionModulus}, {"CentreOfGravityInX", CentreOfGravityInX}, {"CentreOfGravityInY", CentreOfGravityInY}, {"ShearAreaZ", ShearAreaZ}, {"ShearAreaY", ShearAreaY}, {"PlasticShapeFactorY", PlasticShapeFactorY}, {"PlasticShapeFactorZ", PlasticShapeFactorZ}}); }
inline Value IfcStructuralSurfaceConnection(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value AppliedCondition) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralSurfaceConnection", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"AppliedCondition", AppliedCondition}}); }
inline Value IfcStructuralSurfaceMember(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType, Value Thickness) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralSurfaceMember", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}, {"Thickness", Thickness}}); }
inline Value IfcStructuralSurfaceMemberVarying(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value PredefinedType, Value Thickness, Value SubsequentThickness, Value VaryingThicknessLocation) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuralSurfaceMemberVarying", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"PredefinedType", PredefinedType}, {"Thickness", Thickness}, {"SubsequentThickness", SubsequentThickness}, {"VaryingThicknessLocation", VaryingThicknessLocation}}); }
inline Value IfcStructuredDimensionCallout(Value Contents) { return ifcapi::express::make_entity("IFC2X3", "IfcStructuredDimensionCallout", {{"Contents", Contents}}); }
inline Value IfcStyleModel(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC2X3", "IfcStyleModel", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcStyledItem(Value Item, Value Styles, Value Name) { return ifcapi::express::make_entity("IFC2X3", "IfcStyledItem", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}}); }
inline Value IfcStyledRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC2X3", "IfcStyledRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcSubContractResource(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ResourceIdentifier, Value ResourceGroup, Value ResourceConsumption, Value BaseQuantity, Value SubContractor, Value JobDescription) { return ifcapi::express::make_entity("IFC2X3", "IfcSubContractResource", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ResourceIdentifier", ResourceIdentifier}, {"ResourceGroup", ResourceGroup}, {"ResourceConsumption", ResourceConsumption}, {"BaseQuantity", BaseQuantity}, {"SubContractor", SubContractor}, {"JobDescription", JobDescription}}); }
inline Value IfcSubedge(Value EdgeStart, Value EdgeEnd, Value ParentEdge) { return ifcapi::express::make_entity("IFC2X3", "IfcSubedge", {{"EdgeStart", EdgeStart}, {"EdgeEnd", EdgeEnd}, {"ParentEdge", ParentEdge}}); }
inline Value IfcSurface() { return ifcapi::express::make_entity("IFC2X3", "IfcSurface", {}); }
inline Value IfcSurfaceCurveSweptAreaSolid(Value SweptArea, Value Position, Value Directrix, Value StartParam, Value EndParam, Value ReferenceSurface) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceCurveSweptAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}, {"Directrix", Directrix}, {"StartParam", StartParam}, {"EndParam", EndParam}, {"ReferenceSurface", ReferenceSurface}}); }
inline Value IfcSurfaceOfLinearExtrusion(Value SweptCurve, Value Position, Value ExtrudedDirection, Value Depth) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceOfLinearExtrusion", {{"SweptCurve", SweptCurve}, {"Position", Position}, {"ExtrudedDirection", ExtrudedDirection}, {"Depth", Depth}}); }
inline Value IfcSurfaceOfRevolution(Value SweptCurve, Value Position, Value AxisPosition) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceOfRevolution", {{"SweptCurve", SweptCurve}, {"Position", Position}, {"AxisPosition", AxisPosition}}); }
inline Value IfcSurfaceStyle(Value Name, Value Side, Value Styles) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceStyle", {{"Name", Name}, {"Side", Side}, {"Styles", Styles}}); }
inline Value IfcSurfaceStyleLighting(Value DiffuseTransmissionColour, Value DiffuseReflectionColour, Value TransmissionColour, Value ReflectanceColour) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceStyleLighting", {{"DiffuseTransmissionColour", DiffuseTransmissionColour}, {"DiffuseReflectionColour", DiffuseReflectionColour}, {"TransmissionColour", TransmissionColour}, {"ReflectanceColour", ReflectanceColour}}); }
inline Value IfcSurfaceStyleRefraction(Value RefractionIndex, Value DispersionFactor) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceStyleRefraction", {{"RefractionIndex", RefractionIndex}, {"DispersionFactor", DispersionFactor}}); }
inline Value IfcSurfaceStyleRendering(Value SurfaceColour, Value Transparency, Value DiffuseColour, Value TransmissionColour, Value DiffuseTransmissionColour, Value ReflectionColour, Value SpecularColour, Value SpecularHighlight, Value ReflectanceMethod) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceStyleRendering", {{"SurfaceColour", SurfaceColour}, {"Transparency", Transparency}, {"DiffuseColour", DiffuseColour}, {"TransmissionColour", TransmissionColour}, {"DiffuseTransmissionColour", DiffuseTransmissionColour}, {"ReflectionColour", ReflectionColour}, {"SpecularColour", SpecularColour}, {"SpecularHighlight", SpecularHighlight}, {"ReflectanceMethod", ReflectanceMethod}}); }
inline Value IfcSurfaceStyleShading(Value SurfaceColour) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceStyleShading", {{"SurfaceColour", SurfaceColour}}); }
inline Value IfcSurfaceStyleWithTextures(Value Textures) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceStyleWithTextures", {{"Textures", Textures}}); }
inline Value IfcSurfaceTexture(Value RepeatS, Value RepeatT, Value TextureType, Value TextureTransform) { return ifcapi::express::make_entity("IFC2X3", "IfcSurfaceTexture", {{"RepeatS", RepeatS}, {"RepeatT", RepeatT}, {"TextureType", TextureType}, {"TextureTransform", TextureTransform}}); }
inline Value IfcSweptAreaSolid(Value SweptArea, Value Position) { return ifcapi::express::make_entity("IFC2X3", "IfcSweptAreaSolid", {{"SweptArea", SweptArea}, {"Position", Position}}); }
inline Value IfcSweptDiskSolid(Value Directrix, Value Radius, Value InnerRadius, Value StartParam, Value EndParam) { return ifcapi::express::make_entity("IFC2X3", "IfcSweptDiskSolid", {{"Directrix", Directrix}, {"Radius", Radius}, {"InnerRadius", InnerRadius}, {"StartParam", StartParam}, {"EndParam", EndParam}}); }
inline Value IfcSweptSurface(Value SweptCurve, Value Position) { return ifcapi::express::make_entity("IFC2X3", "IfcSweptSurface", {{"SweptCurve", SweptCurve}, {"Position", Position}}); }
inline Value IfcSwitchingDeviceType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcSwitchingDeviceType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcSymbolStyle(Value Name, Value StyleOfSymbol) { return ifcapi::express::make_entity("IFC2X3", "IfcSymbolStyle", {{"Name", Name}, {"StyleOfSymbol", StyleOfSymbol}}); }
inline Value IfcSystem(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcSystem", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }
inline Value IfcSystemFurnitureElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType) { return ifcapi::express::make_entity("IFC2X3", "IfcSystemFurnitureElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}}); }
inline Value IfcTShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value FlangeWidth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value FlangeEdgeRadius, Value WebEdgeRadius, Value WebSlope, Value FlangeSlope, Value CentreOfGravityInY) { return ifcapi::express::make_entity("IFC2X3", "IfcTShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"FlangeWidth", FlangeWidth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"FlangeEdgeRadius", FlangeEdgeRadius}, {"WebEdgeRadius", WebEdgeRadius}, {"WebSlope", WebSlope}, {"FlangeSlope", FlangeSlope}, {"CentreOfGravityInY", CentreOfGravityInY}}); }
inline Value IfcTable(Value Name, Value Rows) { return ifcapi::express::make_entity("IFC2X3", "IfcTable", {{"Name", Name}, {"Rows", Rows}}); }
inline Value IfcTableRow(Value RowCells, Value IsHeading) { return ifcapi::express::make_entity("IFC2X3", "IfcTableRow", {{"RowCells", RowCells}, {"IsHeading", IsHeading}}); }
inline Value IfcTankType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcTankType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTask(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value TaskId, Value Status, Value WorkMethod, Value IsMilestone, Value Priority) { return ifcapi::express::make_entity("IFC2X3", "IfcTask", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"TaskId", TaskId}, {"Status", Status}, {"WorkMethod", WorkMethod}, {"IsMilestone", IsMilestone}, {"Priority", Priority}}); }
inline Value IfcTelecomAddress(Value Purpose, Value Description, Value UserDefinedPurpose, Value TelephoneNumbers, Value FacsimileNumbers, Value PagerNumber, Value ElectronicMailAddresses, Value WWWHomePageURL) { return ifcapi::express::make_entity("IFC2X3", "IfcTelecomAddress", {{"Purpose", Purpose}, {"Description", Description}, {"UserDefinedPurpose", UserDefinedPurpose}, {"TelephoneNumbers", TelephoneNumbers}, {"FacsimileNumbers", FacsimileNumbers}, {"PagerNumber", PagerNumber}, {"ElectronicMailAddresses", ElectronicMailAddresses}, {"WWWHomePageURL", WWWHomePageURL}}); }
inline Value IfcTendon(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade, Value PredefinedType, Value NominalDiameter, Value CrossSectionArea, Value TensionForce, Value PreStress, Value FrictionCoefficient, Value AnchorageSlip, Value MinCurvatureRadius) { return ifcapi::express::make_entity("IFC2X3", "IfcTendon", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}, {"PredefinedType", PredefinedType}, {"NominalDiameter", NominalDiameter}, {"CrossSectionArea", CrossSectionArea}, {"TensionForce", TensionForce}, {"PreStress", PreStress}, {"FrictionCoefficient", FrictionCoefficient}, {"AnchorageSlip", AnchorageSlip}, {"MinCurvatureRadius", MinCurvatureRadius}}); }
inline Value IfcTendonAnchor(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value SteelGrade) { return ifcapi::express::make_entity("IFC2X3", "IfcTendonAnchor", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"SteelGrade", SteelGrade}}); }
inline Value IfcTerminatorSymbol(Value Item, Value Styles, Value Name, Value AnnotatedCurve) { return ifcapi::express::make_entity("IFC2X3", "IfcTerminatorSymbol", {{"Item", Item}, {"Styles", Styles}, {"Name", Name}, {"AnnotatedCurve", AnnotatedCurve}}); }
inline Value IfcTextLiteral(Value Literal, Value Placement, Value Path) { return ifcapi::express::make_entity("IFC2X3", "IfcTextLiteral", {{"Literal", Literal}, {"Placement", Placement}, {"Path", Path}}); }
inline Value IfcTextLiteralWithExtent(Value Literal, Value Placement, Value Path, Value Extent, Value BoxAlignment) { return ifcapi::express::make_entity("IFC2X3", "IfcTextLiteralWithExtent", {{"Literal", Literal}, {"Placement", Placement}, {"Path", Path}, {"Extent", Extent}, {"BoxAlignment", BoxAlignment}}); }
inline Value IfcTextStyle(Value Name, Value TextCharacterAppearance, Value TextStyle, Value TextFontStyle) { return ifcapi::express::make_entity("IFC2X3", "IfcTextStyle", {{"Name", Name}, {"TextCharacterAppearance", TextCharacterAppearance}, {"TextStyle", TextStyle}, {"TextFontStyle", TextFontStyle}}); }
inline Value IfcTextStyleFontModel(Value Name, Value FontFamily, Value FontStyle, Value FontVariant, Value FontWeight, Value FontSize) { return ifcapi::express::make_entity("IFC2X3", "IfcTextStyleFontModel", {{"Name", Name}, {"FontFamily", FontFamily}, {"FontStyle", FontStyle}, {"FontVariant", FontVariant}, {"FontWeight", FontWeight}, {"FontSize", FontSize}}); }
inline Value IfcTextStyleForDefinedFont(Value Colour, Value BackgroundColour) { return ifcapi::express::make_entity("IFC2X3", "IfcTextStyleForDefinedFont", {{"Colour", Colour}, {"BackgroundColour", BackgroundColour}}); }
inline Value IfcTextStyleTextModel(Value TextIndent, Value TextAlign, Value TextDecoration, Value LetterSpacing, Value WordSpacing, Value TextTransform, Value LineHeight) { return ifcapi::express::make_entity("IFC2X3", "IfcTextStyleTextModel", {{"TextIndent", TextIndent}, {"TextAlign", TextAlign}, {"TextDecoration", TextDecoration}, {"LetterSpacing", LetterSpacing}, {"WordSpacing", WordSpacing}, {"TextTransform", TextTransform}, {"LineHeight", LineHeight}}); }
inline Value IfcTextStyleWithBoxCharacteristics(Value BoxHeight, Value BoxWidth, Value BoxSlantAngle, Value BoxRotateAngle, Value CharacterSpacing) { return ifcapi::express::make_entity("IFC2X3", "IfcTextStyleWithBoxCharacteristics", {{"BoxHeight", BoxHeight}, {"BoxWidth", BoxWidth}, {"BoxSlantAngle", BoxSlantAngle}, {"BoxRotateAngle", BoxRotateAngle}, {"CharacterSpacing", CharacterSpacing}}); }
inline Value IfcTextureCoordinate() { return ifcapi::express::make_entity("IFC2X3", "IfcTextureCoordinate", {}); }
inline Value IfcTextureCoordinateGenerator(Value Mode, Value Parameter) { return ifcapi::express::make_entity("IFC2X3", "IfcTextureCoordinateGenerator", {{"Mode", Mode}, {"Parameter", Parameter}}); }
inline Value IfcTextureMap(Value TextureMaps) { return ifcapi::express::make_entity("IFC2X3", "IfcTextureMap", {{"TextureMaps", TextureMaps}}); }
inline Value IfcTextureVertex(Value Coordinates) { return ifcapi::express::make_entity("IFC2X3", "IfcTextureVertex", {{"Coordinates", Coordinates}}); }
inline Value IfcThermalMaterialProperties(Value Material, Value SpecificHeatCapacity, Value BoilingPoint, Value FreezingPoint, Value ThermalConductivity) { return ifcapi::express::make_entity("IFC2X3", "IfcThermalMaterialProperties", {{"Material", Material}, {"SpecificHeatCapacity", SpecificHeatCapacity}, {"BoilingPoint", BoilingPoint}, {"FreezingPoint", FreezingPoint}, {"ThermalConductivity", ThermalConductivity}}); }
inline Value IfcTimeSeries(Value Name, Value Description, Value StartTime, Value EndTime, Value TimeSeriesDataType, Value DataOrigin, Value UserDefinedDataOrigin, Value Unit) { return ifcapi::express::make_entity("IFC2X3", "IfcTimeSeries", {{"Name", Name}, {"Description", Description}, {"StartTime", StartTime}, {"EndTime", EndTime}, {"TimeSeriesDataType", TimeSeriesDataType}, {"DataOrigin", DataOrigin}, {"UserDefinedDataOrigin", UserDefinedDataOrigin}, {"Unit", Unit}}); }
inline Value IfcTimeSeriesReferenceRelationship(Value ReferencedTimeSeries, Value TimeSeriesReferences) { return ifcapi::express::make_entity("IFC2X3", "IfcTimeSeriesReferenceRelationship", {{"ReferencedTimeSeries", ReferencedTimeSeries}, {"TimeSeriesReferences", TimeSeriesReferences}}); }
inline Value IfcTimeSeriesSchedule(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ApplicableDates, Value TimeSeriesScheduleType, Value TimeSeries) { return ifcapi::express::make_entity("IFC2X3", "IfcTimeSeriesSchedule", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ApplicableDates", ApplicableDates}, {"TimeSeriesScheduleType", TimeSeriesScheduleType}, {"TimeSeries", TimeSeries}}); }
inline Value IfcTimeSeriesValue(Value ListValues) { return ifcapi::express::make_entity("IFC2X3", "IfcTimeSeriesValue", {{"ListValues", ListValues}}); }
inline Value IfcTopologicalRepresentationItem() { return ifcapi::express::make_entity("IFC2X3", "IfcTopologicalRepresentationItem", {}); }
inline Value IfcTopologyRepresentation(Value ContextOfItems, Value RepresentationIdentifier, Value RepresentationType, Value Items) { return ifcapi::express::make_entity("IFC2X3", "IfcTopologyRepresentation", {{"ContextOfItems", ContextOfItems}, {"RepresentationIdentifier", RepresentationIdentifier}, {"RepresentationType", RepresentationType}, {"Items", Items}}); }
inline Value IfcTransformerType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcTransformerType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTransportElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value OperationType, Value CapacityByWeight, Value CapacityByNumber) { return ifcapi::express::make_entity("IFC2X3", "IfcTransportElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"OperationType", OperationType}, {"CapacityByWeight", CapacityByWeight}, {"CapacityByNumber", CapacityByNumber}}); }
inline Value IfcTransportElementType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcTransportElementType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTrapeziumProfileDef(Value ProfileType, Value ProfileName, Value Position, Value BottomXDim, Value TopXDim, Value YDim, Value TopXOffset) { return ifcapi::express::make_entity("IFC2X3", "IfcTrapeziumProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"BottomXDim", BottomXDim}, {"TopXDim", TopXDim}, {"YDim", YDim}, {"TopXOffset", TopXOffset}}); }
inline Value IfcTrimmedCurve(Value BasisCurve, Value Trim1, Value Trim2, Value SenseAgreement, Value MasterRepresentation) { return ifcapi::express::make_entity("IFC2X3", "IfcTrimmedCurve", {{"BasisCurve", BasisCurve}, {"Trim1", Trim1}, {"Trim2", Trim2}, {"SenseAgreement", SenseAgreement}, {"MasterRepresentation", MasterRepresentation}}); }
inline Value IfcTubeBundleType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcTubeBundleType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcTwoDirectionRepeatFactor(Value RepeatFactor, Value SecondRepeatFactor) { return ifcapi::express::make_entity("IFC2X3", "IfcTwoDirectionRepeatFactor", {{"RepeatFactor", RepeatFactor}, {"SecondRepeatFactor", SecondRepeatFactor}}); }
inline Value IfcTypeObject(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets) { return ifcapi::express::make_entity("IFC2X3", "IfcTypeObject", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}}); }
inline Value IfcTypeProduct(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcTypeProduct", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}}); }
inline Value IfcUShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value FlangeWidth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value EdgeRadius, Value FlangeSlope, Value CentreOfGravityInX) { return ifcapi::express::make_entity("IFC2X3", "IfcUShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"FlangeWidth", FlangeWidth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"EdgeRadius", EdgeRadius}, {"FlangeSlope", FlangeSlope}, {"CentreOfGravityInX", CentreOfGravityInX}}); }
inline Value IfcUnitAssignment(Value Units) { return ifcapi::express::make_entity("IFC2X3", "IfcUnitAssignment", {{"Units", Units}}); }
inline Value IfcUnitaryEquipmentType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcUnitaryEquipmentType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcValveType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcValveType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcVector(Value Orientation, Value Magnitude) { return ifcapi::express::make_entity("IFC2X3", "IfcVector", {{"Orientation", Orientation}, {"Magnitude", Magnitude}}); }
inline Value IfcVertex() { return ifcapi::express::make_entity("IFC2X3", "IfcVertex", {}); }
inline Value IfcVertexBasedTextureMap(Value TextureVertices, Value TexturePoints) { return ifcapi::express::make_entity("IFC2X3", "IfcVertexBasedTextureMap", {{"TextureVertices", TextureVertices}, {"TexturePoints", TexturePoints}}); }
inline Value IfcVertexLoop(Value LoopVertex) { return ifcapi::express::make_entity("IFC2X3", "IfcVertexLoop", {{"LoopVertex", LoopVertex}}); }
inline Value IfcVertexPoint(Value VertexGeometry) { return ifcapi::express::make_entity("IFC2X3", "IfcVertexPoint", {{"VertexGeometry", VertexGeometry}}); }
inline Value IfcVibrationIsolatorType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcVibrationIsolatorType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcVirtualElement(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcVirtualElement", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcVirtualGridIntersection(Value IntersectingAxes, Value OffsetDistances) { return ifcapi::express::make_entity("IFC2X3", "IfcVirtualGridIntersection", {{"IntersectingAxes", IntersectingAxes}, {"OffsetDistances", OffsetDistances}}); }
inline Value IfcWall(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcWall", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcWallStandardCase(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag) { return ifcapi::express::make_entity("IFC2X3", "IfcWallStandardCase", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}}); }
inline Value IfcWallType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcWallType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWasteTerminalType(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ElementType, Value PredefinedType) { return ifcapi::express::make_entity("IFC2X3", "IfcWasteTerminalType", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ElementType", ElementType}, {"PredefinedType", PredefinedType}}); }
inline Value IfcWaterProperties(Value Material, Value IsPotable, Value Hardness, Value AlkalinityConcentration, Value AcidityConcentration, Value ImpuritiesContent, Value PHLevel, Value DissolvedSolidsContent) { return ifcapi::express::make_entity("IFC2X3", "IfcWaterProperties", {{"Material", Material}, {"IsPotable", IsPotable}, {"Hardness", Hardness}, {"AlkalinityConcentration", AlkalinityConcentration}, {"AcidityConcentration", AcidityConcentration}, {"ImpuritiesContent", ImpuritiesContent}, {"PHLevel", PHLevel}, {"DissolvedSolidsContent", DissolvedSolidsContent}}); }
inline Value IfcWindow(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value ObjectPlacement, Value Representation, Value Tag, Value OverallHeight, Value OverallWidth) { return ifcapi::express::make_entity("IFC2X3", "IfcWindow", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"ObjectPlacement", ObjectPlacement}, {"Representation", Representation}, {"Tag", Tag}, {"OverallHeight", OverallHeight}, {"OverallWidth", OverallWidth}}); }
inline Value IfcWindowLiningProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value LiningDepth, Value LiningThickness, Value TransomThickness, Value MullionThickness, Value FirstTransomOffset, Value SecondTransomOffset, Value FirstMullionOffset, Value SecondMullionOffset, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC2X3", "IfcWindowLiningProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"LiningDepth", LiningDepth}, {"LiningThickness", LiningThickness}, {"TransomThickness", TransomThickness}, {"MullionThickness", MullionThickness}, {"FirstTransomOffset", FirstTransomOffset}, {"SecondTransomOffset", SecondTransomOffset}, {"FirstMullionOffset", FirstMullionOffset}, {"SecondMullionOffset", SecondMullionOffset}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcWindowPanelProperties(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value OperationType, Value PanelPosition, Value FrameDepth, Value FrameThickness, Value ShapeAspectStyle) { return ifcapi::express::make_entity("IFC2X3", "IfcWindowPanelProperties", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"OperationType", OperationType}, {"PanelPosition", PanelPosition}, {"FrameDepth", FrameDepth}, {"FrameThickness", FrameThickness}, {"ShapeAspectStyle", ShapeAspectStyle}}); }
inline Value IfcWindowStyle(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ApplicableOccurrence, Value HasPropertySets, Value RepresentationMaps, Value Tag, Value ConstructionType, Value OperationType, Value ParameterTakesPrecedence, Value Sizeable) { return ifcapi::express::make_entity("IFC2X3", "IfcWindowStyle", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ApplicableOccurrence", ApplicableOccurrence}, {"HasPropertySets", HasPropertySets}, {"RepresentationMaps", RepresentationMaps}, {"Tag", Tag}, {"ConstructionType", ConstructionType}, {"OperationType", OperationType}, {"ParameterTakesPrecedence", ParameterTakesPrecedence}, {"Sizeable", Sizeable}}); }
inline Value IfcWorkControl(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identifier, Value CreationDate, Value Creators, Value Purpose, Value Duration, Value TotalFloat, Value StartTime, Value FinishTime, Value WorkControlType, Value UserDefinedControlType) { return ifcapi::express::make_entity("IFC2X3", "IfcWorkControl", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identifier", Identifier}, {"CreationDate", CreationDate}, {"Creators", Creators}, {"Purpose", Purpose}, {"Duration", Duration}, {"TotalFloat", TotalFloat}, {"StartTime", StartTime}, {"FinishTime", FinishTime}, {"WorkControlType", WorkControlType}, {"UserDefinedControlType", UserDefinedControlType}}); }
inline Value IfcWorkPlan(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identifier, Value CreationDate, Value Creators, Value Purpose, Value Duration, Value TotalFloat, Value StartTime, Value FinishTime, Value WorkControlType, Value UserDefinedControlType) { return ifcapi::express::make_entity("IFC2X3", "IfcWorkPlan", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identifier", Identifier}, {"CreationDate", CreationDate}, {"Creators", Creators}, {"Purpose", Purpose}, {"Duration", Duration}, {"TotalFloat", TotalFloat}, {"StartTime", StartTime}, {"FinishTime", FinishTime}, {"WorkControlType", WorkControlType}, {"UserDefinedControlType", UserDefinedControlType}}); }
inline Value IfcWorkSchedule(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType, Value Identifier, Value CreationDate, Value Creators, Value Purpose, Value Duration, Value TotalFloat, Value StartTime, Value FinishTime, Value WorkControlType, Value UserDefinedControlType) { return ifcapi::express::make_entity("IFC2X3", "IfcWorkSchedule", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}, {"Identifier", Identifier}, {"CreationDate", CreationDate}, {"Creators", Creators}, {"Purpose", Purpose}, {"Duration", Duration}, {"TotalFloat", TotalFloat}, {"StartTime", StartTime}, {"FinishTime", FinishTime}, {"WorkControlType", WorkControlType}, {"UserDefinedControlType", UserDefinedControlType}}); }
inline Value IfcZShapeProfileDef(Value ProfileType, Value ProfileName, Value Position, Value Depth, Value FlangeWidth, Value WebThickness, Value FlangeThickness, Value FilletRadius, Value EdgeRadius) { return ifcapi::express::make_entity("IFC2X3", "IfcZShapeProfileDef", {{"ProfileType", ProfileType}, {"ProfileName", ProfileName}, {"Position", Position}, {"Depth", Depth}, {"FlangeWidth", FlangeWidth}, {"WebThickness", WebThickness}, {"FlangeThickness", FlangeThickness}, {"FilletRadius", FilletRadius}, {"EdgeRadius", EdgeRadius}}); }
inline Value IfcZone(Value GlobalId, Value OwnerHistory, Value Name, Value Description, Value ObjectType) { return ifcapi::express::make_entity("IFC2X3", "IfcZone", {{"GlobalId", GlobalId}, {"OwnerHistory", OwnerHistory}, {"Name", Name}, {"Description", Description}, {"ObjectType", ObjectType}}); }

// --- function forward declarations ---
Value IfcAddToBeginOfList(Value, Value, Value);
Value IfcBaseAxis(Value, Value, Value, Value);
Value IfcBooleanChoose(Value, Value, Value, Value);
Value IfcBuild2Axes(Value);
Value IfcBuildAxes(Value, Value);
Value IfcCorrectDimensions(Value, Value);
Value IfcCorrectFillAreaStyle(Value);
Value IfcCorrectLocalPlacement(Value, Value);
Value IfcCorrectObjectAssignment(Value, Value);
Value IfcCorrectUnitAssignment(Value);
Value IfcCrossProduct(Value, Value);
Value IfcCurveDim(Value);
Value IfcCurveWeightsPositive(Value);
Value IfcDeriveDimensionalExponents(Value);
Value IfcDimensionsForSiUnit(Value);
Value IfcDotProduct(Value, Value);
Value IfcFirstProjAxis(Value, Value);
Value IfcLeapYear(Value);
Value IfcListToArray(Value, Value, Value, Value);
Value IfcLoopHeadToTail(Value);
Value IfcMlsTotalThickness(Value);
Value IfcNormalise(Value);
Value IfcOrthogonalComplement(Value);
Value IfcPathHeadToTail(Value);
Value IfcSameAxis2Placement(Value, Value, Value);
Value IfcSameCartesianPoint(Value, Value, Value);
Value IfcSameDirection(Value, Value, Value);
Value IfcSameValidPrecision(Value, Value);
Value IfcSameValue(Value, Value, Value);
Value IfcScalarTimesVector(Value, Value);
Value IfcSecondProjAxis(Value, Value, Value);
Value IfcShapeRepresentationTypes(Value, Value);
Value IfcTopologyRepresentationTypes(Value, Value);
Value IfcUniquePropertyName(Value);
Value IfcValidCalendarDate(Value);
Value IfcValidTime(Value);
Value IfcVectorDifference(Value, Value);
Value IfcVectorSum(Value, Value);

// --- function definitions ---
Value IfcAddToBeginOfList(Value ascalar, Value alist) {
    Value result = Value::make_list({});
    if (Value(!(exists(ascalar))).truthy()) {
        result = alist;
    } else {
        result = result + ascalar;
        if (Value(hiindex(alist) >= 1).truthy()) {
            for (Value i = 1; (Value(i) <= Value(hiindex(alist))).truthy(); i = i + Value((int64_t)1)) {
                result = ifcapi::express::set_index(result, i + 1 - 1, express_getitem(alist, i - 1));
            }
        }
    }
    return result;
    return Value();
}

Value IfcBaseAxis(Value dim, Value axis1, Value axis2, Value axis3) {
    Value d1;
    Value d2;
    Value u;
    Value factor;
    if (Value(dim == 3).truthy()) {
        d1 = nvl(IfcNormalise(axis3), IfcDirection(Value::make_list({0.0, 0.0, 1.0})));
        d2 = IfcFirstProjAxis(d1, axis1);
        u = Value::make_list({d2, IfcSecondProjAxis(d1, d2, axis2), d1});
    } else {
        if (Value(exists(axis1)).truthy()) {
            d1 = IfcNormalise(axis1);
            u = Value::make_list({d1, IfcOrthogonalComplement(d1)});
            if (Value(exists(axis2)).truthy()) {
                factor = IfcDotProduct(axis2, express_getitem(u, 2 - 1));
                if (Value(factor < 0.0).truthy()) {
                    u = ifcapi::express::set_index(u, 2 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios"), 1 - 1, -express_getitem(express_getattr(express_getitem(u, 2 - 1), "DirectionRatios"), 1 - 1))));
                    u = ifcapi::express::set_index(u, 2 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 2 - 1), "DirectionRatios"), 2 - 1, -express_getitem(express_getattr(express_getitem(u, 2 - 1), "DirectionRatios"), 2 - 1))));
                }
            }
        } else {
            if (Value(exists(axis2)).truthy()) {
                d1 = IfcNormalise(axis2);
                u = Value::make_list({IfcOrthogonalComplement(d1), d1});
                u = ifcapi::express::set_index(u, 1 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios"), 1 - 1, -express_getitem(express_getattr(express_getitem(u, 1 - 1), "DirectionRatios"), 1 - 1))));
                u = ifcapi::express::set_index(u, 1 - 1, ifcapi::express::set_attr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(ifcapi::express::express_getitem(u, 1 - 1), "DirectionRatios"), 2 - 1, -express_getitem(express_getattr(express_getitem(u, 1 - 1), "DirectionRatios"), 2 - 1))));
            } else {
                u = Value::make_list({IfcDirection(Value::make_list({1.0, 0.0})), IfcDirection(Value::make_list({0.0, 1.0}))});
            }
        }
    }
    return u;
    return Value();
}

Value IfcBooleanChoose(Value b, Value choice1, Value choice2) {
    if (Value(b).truthy()) {
        return choice1;
    } else {
        return choice2;
    }
    return Value();
}

Value IfcBuild2Axes(Value refdirection) {
    Value d = nvl(IfcNormalise(refdirection), IfcDirection(Value::make_list({1.0, 0.0})));
    return Value::make_list({d, IfcOrthogonalComplement(d)});
    return Value();
}

Value IfcBuildAxes(Value axis, Value refdirection) {
    Value d1;
    Value d2;
    d1 = nvl(IfcNormalise(axis), IfcDirection(Value::make_list({0.0, 0.0, 1.0})));
    d2 = IfcFirstProjAxis(d1, refdirection);
    return Value::make_list({d2, express_getattr(IfcNormalise(IfcCrossProduct(d1, d2)), "Orientation"), d1});
    return Value();
}

Value IfcCorrectDimensions(Value m, Value dim) {
    if (Value((m) == lengthunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(1, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == massunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 1, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == timeunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 1, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electriccurrentunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == thermodynamictemperatureunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 1, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == amountofsubstanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 1, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == luminousintensityunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == planeangleunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == solidangleunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == areaunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == volumeunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(3, 0, 0, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == absorbeddoseunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == radioactivityunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electriccapacitanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-2, 1, 4, 1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == doseequivalentunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricchargeunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 1, 1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricconductanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-2, -1, 3, 2, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricvoltageunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -3, -1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == electricresistanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -3, -2, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == energyunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == forceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(1, 1, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == frequencyunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == inductanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -2, -2, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == illuminanceunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-2, 0, 0, 0, 0, 0, 1))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == luminousfluxunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == magneticfluxunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -2, -1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == magneticfluxdensityunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(0, 1, -2, -1, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == powerunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(2, 1, -3, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    
    } else if (Value((m) == pressureunit).truthy()) {
        if (Value(dim == (IfcDimensionalExponents(-1, 1, -2, 0, 0, 0, 0))).truthy()) {
            return true;
        } else {
            return false;
        }
    } else {
        return unknown;
    }
    return Value();
}

Value IfcCorrectFillAreaStyle(Value styles) {
    Value hatching = 0;
    Value tiles = 0;
    Value colour = 0;
    Value external = 0;
    external = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcexternallydefinedhatchstyle")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    hatching = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfillareastylehatching")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    tiles = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfillareastyletiles")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    colour = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(styles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifccolour")), typeof_(style))).truthy()) __r.append(style); } return __r; })());
    if (Value(external > 1).truthy()) {
        return false;
    }
    if (Value((external == 1) && ((hatching > 0) || (tiles > 0) || (colour > 0))).truthy()) {
        return false;
    }
    if (Value(colour > 1).truthy()) {
        return false;
    }
    if (Value((hatching > 0) && (tiles > 0)).truthy()) {
        return false;
    }
    return true;
    return Value();
}

Value IfcCorrectLocalPlacement(Value axisplacement, Value relplacement) {
    if (Value(exists(relplacement)).truthy()) {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgridplacement")), typeof_(relplacement))).truthy()) {
            return Value();
        }
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifclocalplacement")), typeof_(relplacement))).truthy()) {
            if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcaxis2placement2d")), typeof_(axisplacement))).truthy()) {
                return true;
            }
            if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcaxis2placement3d")), typeof_(axisplacement))).truthy()) {
                if (Value((express_getattr(express_getattr(relplacement, "RelativePlacement"), "Dim")) == 3).truthy()) {
                    return true;
                } else {
                    return false;
                }
            }
        }
        return true;
    }
    return Value();
    return Value();
}

Value IfcCorrectObjectAssignment(Value constraint, Value objects) {
    Value count = 0;
    if (Value(!(exists(constraint))).truthy()) {
        return true;
    }
    if (Value((constraint) == Value(std::string("NOTDEFINED"))).truthy()) {
        return true;
    
    } else if (Value((constraint) == Value(std::string("PRODUCT"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcproduct")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("PROCESS"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcprocess")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("CONTROL"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifccontrol")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("RESOURCE"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcresource")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("ACTOR"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcactor")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("GROUP"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgroup")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    
    } else if (Value((constraint) == Value(std::string("PROJECT"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(objects)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcproject")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })());
        return count == 0;
    } else {
        return Value();
    }
    return Value();
}

Value IfcCorrectUnitAssignment(Value units) {
    Value namedunitnumber = 0;
    Value derivedunitnumber = 0;
    Value monetaryunitnumber = 0;
    Value namedunitnames = Value::make_list({});
    Value derivedunitnames = Value::make_list({});
    namedunitnumber = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(units)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcnamedunit")), typeof_(temp))) && (!(((express_getattr(temp, "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) __r.append(temp); } return __r; })());
    derivedunitnumber = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(units)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcderivedunit")), typeof_(temp))) && (!(((express_getattr(temp, "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) __r.append(temp); } return __r; })());
    monetaryunitnumber = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(units)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcmonetaryunit")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    for (Value i = 1; (Value(i) <= Value(sizeof_(units))).truthy(); i = i + Value((int64_t)1)) {
        if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcnamedunit")), typeof_(express_getitem(units, i - 1)))) && (!(((express_getattr(express_getitem(units, i - 1), "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) {
            namedunitnames = namedunitnames + (express_getattr(express_getitem(units, i - 1), "UnitType"));
        }
        if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcderivedunit")), typeof_(express_getitem(units, i - 1)))) && (!(((express_getattr(express_getitem(units, i - 1), "UnitType")) == Value(std::string("USERDEFINED")))))).truthy()) {
            derivedunitnames = derivedunitnames + (express_getattr(express_getitem(units, i - 1), "UnitType"));
        }
    }
    return (sizeof_(namedunitnames) == namedunitnumber) && (sizeof_(derivedunitnames) == derivedunitnumber) && (monetaryunitnumber <= 1);
    return Value();
}

Value IfcCrossProduct(Value arg1, Value arg2) {
    Value v1;
    Value v2;
    Value res;
    Value mag;
    Value result;
    if (Value((!(exists(arg1)) || ((express_getattr(arg1, "Dim")) == 2)) || (!(exists(arg2)) || ((express_getattr(arg2, "Dim")) == 2))).truthy()) {
        return Value();
    } else {
        v1 = express_getattr(IfcNormalise(arg1), "DirectionRatios");
        v2 = express_getattr(IfcNormalise(arg2), "DirectionRatios");
        res = IfcDirection(Value::make_list({((express_getitem(v1, 2 - 1)) * (express_getitem(v2, 3 - 1))) - ((express_getitem(v1, 3 - 1)) * (express_getitem(v2, 2 - 1))), ((express_getitem(v1, 3 - 1)) * (express_getitem(v2, 1 - 1))) - ((express_getitem(v1, 1 - 1)) * (express_getitem(v2, 3 - 1))), ((express_getitem(v1, 1 - 1)) * (express_getitem(v2, 2 - 1))) - ((express_getitem(v1, 2 - 1)) * (express_getitem(v2, 1 - 1)))}));
        mag = 0.0;
        for (Value i = 1; (Value(i) <= Value(3)).truthy(); i = i + Value((int64_t)1)) {
            mag = mag + ((express_getitem(express_getattr(res, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(res, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            result = IfcVector(res,  math_sqrt(mag));
        } else {
            result = IfcVector(arg1,  0.0);
        }
        return result;
    }
    return Value();
}

Value IfcCurveDim(Value curve) {
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcline")), typeof_(curve))).truthy()) {
        return express_getattr(express_getattr(curve, "Pnt"), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcconic")), typeof_(curve))).truthy()) {
        return express_getattr(express_getattr(curve, "Position"), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcpolyline")), typeof_(curve))).truthy()) {
        return express_getattr(express_getitem(express_getattr(curve, "Points"), 1 - 1), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifctrimmedcurve")), typeof_(curve))).truthy()) {
        return IfcCurveDim(express_getattr(curve, "BasisCurve"));
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifccompositecurve")), typeof_(curve))).truthy()) {
        return express_getattr(express_getitem(express_getattr(curve, "Segments"), 1 - 1), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcbsplinecurve")), typeof_(curve))).truthy()) {
        return express_getattr(express_getitem(express_getattr(curve, "ControlPointsList"), 1 - 1), "Dim");
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcoffsetcurve2d")), typeof_(curve))).truthy()) {
        return 2;
    }
    if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcoffsetcurve3d")), typeof_(curve))).truthy()) {
        return 3;
    }
    return Value();
    return Value();
}

Value IfcCurveWeightsPositive(Value b) {
    Value result = true;
    for (Value i = 0; (Value(i) <= Value(express_getattr(b, "UpperIndexOnControlPoints"))).truthy(); i = i + Value((int64_t)1)) {
        if (Value((express_getitem(express_getattr(b, "Weights"), i - 1)) <= 0.0).truthy()) {
            result = false;
            return result;
        }
    }
    return result;
    return Value();
}

Value IfcDeriveDimensionalExponents(Value unitelements) {
    Value result = IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    for (Value i = loindex(unitelements); (Value(i) <= Value(hiindex(unitelements))).truthy(); i = i + Value((int64_t)1)) {
        result = ifcapi::express::set_attr(result, "LengthExponent", (express_getattr(result, "LengthExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "LengthExponent"))));
        result = ifcapi::express::set_attr(result, "MassExponent", (express_getattr(result, "MassExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "MassExponent"))));
        result = ifcapi::express::set_attr(result, "TimeExponent", (express_getattr(result, "TimeExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "TimeExponent"))));
        result = ifcapi::express::set_attr(result, "ElectricCurrentExponent", (express_getattr(result, "ElectricCurrentExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "ElectricCurrentExponent"))));
        result = ifcapi::express::set_attr(result, "ThermodynamicTemperatureExponent", (express_getattr(result, "ThermodynamicTemperatureExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "ThermodynamicTemperatureExponent"))));
        result = ifcapi::express::set_attr(result, "AmountOfSubstanceExponent", (express_getattr(result, "AmountOfSubstanceExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "AmountOfSubstanceExponent"))));
        result = ifcapi::express::set_attr(result, "LuminousIntensityExponent", (express_getattr(result, "LuminousIntensityExponent")) + ((express_getattr(express_getitem(unitelements, i - 1), "Exponent")) * (express_getattr(express_getattr(express_getattr(express_getitem(unitelements, i - 1), "Unit"), "Dimensions"), "LuminousIntensityExponent"))));
    }
    return result;
    return Value();
}

Value IfcDimensionsForSiUnit(Value n) {
    if (Value((n) == metre).truthy()) {
        return IfcDimensionalExponents(1, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == square_metre).truthy()) {
        return IfcDimensionalExponents(2, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == cubic_metre).truthy()) {
        return IfcDimensionalExponents(3, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == gram).truthy()) {
        return IfcDimensionalExponents(0, 1, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == second).truthy()) {
        return IfcDimensionalExponents(0, 0, 1, 0, 0, 0, 0);
    
    } else if (Value((n) == ampere).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 1, 0, 0, 0);
    
    } else if (Value((n) == kelvin).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 1, 0, 0);
    
    } else if (Value((n) == mole).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 1, 0);
    
    } else if (Value((n) == candela).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1);
    
    } else if (Value((n) == radian).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == steradian).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    
    } else if (Value((n) == hertz).truthy()) {
        return IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0);
    
    } else if (Value((n) == newton).truthy()) {
        return IfcDimensionalExponents(1, 1, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == pascal).truthy()) {
        return IfcDimensionalExponents(-1, 1, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == joule).truthy()) {
        return IfcDimensionalExponents(2, 1, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == watt).truthy()) {
        return IfcDimensionalExponents(2, 1, -3, 0, 0, 0, 0);
    
    } else if (Value((n) == coulomb).truthy()) {
        return IfcDimensionalExponents(0, 0, 1, 1, 0, 0, 0);
    
    } else if (Value((n) == volt).truthy()) {
        return IfcDimensionalExponents(2, 1, -3, -1, 0, 0, 0);
    
    } else if (Value((n) == farad).truthy()) {
        return IfcDimensionalExponents(-2, -1, 4, 1, 0, 0, 0);
    
    } else if (Value((n) == ohm).truthy()) {
        return IfcDimensionalExponents(2, 1, -3, -2, 0, 0, 0);
    
    } else if (Value((n) == siemens).truthy()) {
        return IfcDimensionalExponents(-2, -1, 3, 2, 0, 0, 0);
    
    } else if (Value((n) == weber).truthy()) {
        return IfcDimensionalExponents(2, 1, -2, -1, 0, 0, 0);
    
    } else if (Value((n) == tesla).truthy()) {
        return IfcDimensionalExponents(0, 1, -2, -1, 0, 0, 0);
    
    } else if (Value((n) == henry).truthy()) {
        return IfcDimensionalExponents(2, 1, -2, -2, 0, 0, 0);
    
    } else if (Value((n) == degree_celsius).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 1, 0, 0);
    
    } else if (Value((n) == lumen).truthy()) {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 1);
    
    } else if (Value((n) == lux).truthy()) {
        return IfcDimensionalExponents(-2, 0, 0, 0, 0, 0, 1);
    
    } else if (Value((n) == becquerel).truthy()) {
        return IfcDimensionalExponents(0, 0, -1, 0, 0, 0, 0);
    
    } else if (Value((n) == gray).truthy()) {
        return IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0);
    
    } else if (Value((n) == sievert).truthy()) {
        return IfcDimensionalExponents(2, 0, -2, 0, 0, 0, 0);
    } else {
        return IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
    }
    return Value();
}

Value IfcDotProduct(Value arg1, Value arg2) {
    Value scalar;
    Value vec1;
    Value vec2;
    Value ndim;
    if (Value(!(exists(arg1)) || !(exists(arg2))).truthy()) {
        scalar = Value();
    } else {
        if (Value((express_getattr(arg1, "Dim")) != (express_getattr(arg2, "Dim"))).truthy()) {
            scalar = Value();
        } else {
            vec1 = IfcNormalise(arg1);
            vec2 = IfcNormalise(arg2);
            ndim = express_getattr(arg1, "Dim");
            scalar = 0.0;
            for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
                scalar = scalar + ((express_getitem(express_getattr(vec1, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(vec2, "DirectionRatios"), i - 1)));
            }
        }
    }
    return scalar;
    return Value();
}

Value IfcFirstProjAxis(Value zaxis, Value arg) {
    Value z;
    Value v;
    Value xvec;
    Value xaxis;
    if (Value(!(exists(zaxis))).truthy()) {
        return Value();
    } else {
        z = IfcNormalise(zaxis);
        if (Value(!(exists(arg))).truthy()) {
            if (Value((express_getattr(z, "DirectionRatios")) != (Value::make_list({1.0, 0.0, 0.0}))).truthy()) {
                v = IfcDirection(Value::make_list({1.0, 0.0, 0.0}));
            } else {
                v = IfcDirection(Value::make_list({0.0, 1.0, 0.0}));
            }
        } else {
            if (Value((express_getattr(arg, "Dim")) != 3).truthy()) {
                return Value();
            }
            if (Value((express_getattr(IfcCrossProduct(arg, z), "Magnitude")) == 0.0).truthy()) {
                return Value();
            } else {
                v = IfcNormalise(arg);
            }
        }
        xvec = IfcScalarTimesVector(IfcDotProduct(v, z), z);
        xaxis = express_getattr(IfcVectorDifference(v, xvec), "Orientation");
        xaxis = IfcNormalise(xaxis);
    }
    return xaxis;
    return Value();
}

Value IfcLeapYear(Value year) {
    if (Value((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0)).truthy()) {
        return true;
    } else {
        return false;
    }
    return Value();
}

Value IfcListToArray(Value lis, Value low, Value u) {
    Value n;
    Value res;
    n = sizeof_(lis);
    if (Value(n != (u - low + 1)).truthy()) {
        return Value();
    } else {
        res = ifcapi::express::repeat(express_getitem(lis, 1 - 1), n);
        for (Value i = 2; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
            res = ifcapi::express::set_index(res, low + i - 1 - 1, express_getitem(lis, i - 1));
        }
        return res;
    }
    return Value();
}

Value IfcLoopHeadToTail(Value aloop) {
    Value n;
    
    Value p = true;
    n = sizeof_(express_getattr(aloop, "EdgeList"));
    for (Value i = 2; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
        p = p && ((express_getattr(express_getitem(express_getattr(aloop, "EdgeList"), i - 1 - 1), "EdgeEnd")) == (express_getattr(express_getitem(express_getattr(aloop, "EdgeList"), i - 1), "EdgeStart")));
    }
    return p;
    return Value();
}

Value IfcMlsTotalThickness(Value layerset) {
    Value max = express_getattr(express_getitem(express_getattr(layerset, "MaterialLayers"), 1 - 1), "LayerThickness");
    if (Value((sizeof_(express_getattr(layerset, "MaterialLayers"))) > 1).truthy()) {
        for (Value i = 2; (Value(i) <= Value(hiindex(express_getattr(layerset, "MaterialLayers")))).truthy(); i = i + Value((int64_t)1)) {
            max = max + (express_getattr(express_getitem(express_getattr(layerset, "MaterialLayers"), i - 1), "LayerThickness"));
        }
    }
    return max;
    return Value();
}

Value IfcNormalise(Value arg) {
    Value ndim;
    Value mag;
    
    Value v = IfcDirection(Value::make_list({1., 0.}));
    Value vec = IfcVector(IfcDirection(Value::make_list({1., 0.})),  1.);
    
    Value result = v;
    if (Value(!(exists(arg))).truthy()) {
        return Value();
    } else {
        ndim = express_getattr(arg, "Dim");
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvector")), typeof_(arg))).truthy()) {
            v = ifcapi::express::set_attr(v, "DirectionRatios", express_getattr(express_getattr(arg, "Orientation"), "DirectionRatios"));
            vec = ifcapi::express::set_attr(vec, "Magnitude", express_getattr(arg, "Magnitude"));
            vec = ifcapi::express::set_attr(vec, "Orientation", v);
            if (Value((express_getattr(arg, "Magnitude")) == 0.0).truthy()) {
                return Value();
            } else {
                vec = ifcapi::express::set_attr(vec, "Magnitude", 1.0);
            }
        } else {
            v = ifcapi::express::set_attr(v, "DirectionRatios", express_getattr(arg, "DirectionRatios"));
        }
        mag = 0.0;
        for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
            mag = mag + ((express_getitem(express_getattr(v, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(v, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            mag = math_sqrt(mag);
            for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
                v = ifcapi::express::set_attr(v, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(v, "DirectionRatios"), i - 1, (express_getitem(express_getattr(v, "DirectionRatios"), i - 1)) / mag));
            }
            if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvector")), typeof_(arg))).truthy()) {
                vec = ifcapi::express::set_attr(vec, "Orientation", v);
                result = vec;
            } else {
                result = v;
            }
        } else {
            return Value();
        }
    }
    return result;
    return Value();
}

Value IfcOrthogonalComplement(Value vec) {
    Value result;
    if (Value(!(exists(vec)) || ((express_getattr(vec, "Dim")) != 2)).truthy()) {
        return Value();
    } else {
        result = IfcDirection(Value::make_list({-express_getitem(express_getattr(vec, "DirectionRatios"), 2 - 1), express_getitem(express_getattr(vec, "DirectionRatios"), 1 - 1)}));
        return result;
    }
    return Value();
}

Value IfcPathHeadToTail(Value apath) {
    Value n = 0;
    Value p = unknown;
    n = sizeof_(express_getattr(apath, "EdgeList"));
    for (Value i = 2; (Value(i) <= Value(n)).truthy(); i = i + Value((int64_t)1)) {
        p = p && ((express_getattr(express_getitem(express_getattr(apath, "EdgeList"), i - 1 - 1), "EdgeEnd")) == (express_getattr(express_getitem(express_getattr(apath, "EdgeList"), i - 1), "EdgeStart")));
    }
    return p;
    return Value();
}

Value IfcSameAxis2Placement(Value ap1, Value ap2, Value epsilon) {
    return (IfcSameDirection(express_getitem(express_getattr(ap1, "P"), 1 - 1), express_getitem(express_getattr(ap2, "P"), 1 - 1), epsilon)) && (IfcSameDirection(express_getitem(express_getattr(ap1, "P"), 2 - 1), express_getitem(express_getattr(ap2, "P"), 2 - 1), epsilon)) && (IfcSameCartesianPoint(express_getattr(ap1, "Location"), express_getattr(ap1, "Location"), epsilon));
    return Value();
}

Value IfcSameCartesianPoint(Value cp1, Value cp2, Value epsilon) {
    Value cp1x = express_getitem(express_getattr(cp1, "Coordinates"), 1 - 1);
    Value cp1y = express_getitem(express_getattr(cp1, "Coordinates"), 2 - 1);
    Value cp1z = 0;
    Value cp2x = express_getitem(express_getattr(cp2, "Coordinates"), 1 - 1);
    Value cp2y = express_getitem(express_getattr(cp2, "Coordinates"), 2 - 1);
    Value cp2z = 0;
    if (Value((sizeof_(express_getattr(cp1, "Coordinates"))) > 2).truthy()) {
        cp1z = express_getitem(express_getattr(cp1, "Coordinates"), 3 - 1);
    }
    if (Value((sizeof_(express_getattr(cp2, "Coordinates"))) > 2).truthy()) {
        cp2z = express_getitem(express_getattr(cp2, "Coordinates"), 3 - 1);
    }
    return (IfcSameValue(cp1x, cp2x, epsilon)) && (IfcSameValue(cp1y, cp2y, epsilon)) && (IfcSameValue(cp1z, cp2z, epsilon));
    return Value();
}

Value IfcSameDirection(Value dir1, Value dir2, Value epsilon) {
    Value dir1x = express_getitem(express_getattr(dir1, "DirectionRatios"), 1 - 1);
    Value dir1y = express_getitem(express_getattr(dir1, "DirectionRatios"), 2 - 1);
    Value dir1z = 0;
    Value dir2x = express_getitem(express_getattr(dir2, "DirectionRatios"), 1 - 1);
    Value dir2y = express_getitem(express_getattr(dir2, "DirectionRatios"), 2 - 1);
    Value dir2z = 0;
    if (Value((sizeof_(express_getattr(dir1, "DirectionRatios"))) > 2).truthy()) {
        dir1z = express_getitem(express_getattr(dir1, "DirectionRatios"), 3 - 1);
    }
    if (Value((sizeof_(express_getattr(dir2, "DirectionRatios"))) > 2).truthy()) {
        dir2z = express_getitem(express_getattr(dir2, "DirectionRatios"), 3 - 1);
    }
    return (IfcSameValue(dir1x, dir2x, epsilon)) && (IfcSameValue(dir1y, dir2y, epsilon)) && (IfcSameValue(dir1z, dir2z, epsilon));
    return Value();
}

Value IfcSameValidPrecision(Value epsilon1, Value epsilon2) {
    Value valideps1;
    Value valideps2;
    
    Value defaulteps = 0.000001;
    Value derivationofeps = 1.001;
    Value uppereps = 1.0;
    valideps1 = nvl(epsilon1, defaulteps);
    valideps2 = nvl(epsilon2, defaulteps);
    return (0.0 < valideps1) && (valideps1 <= (derivationofeps * valideps2)) && (valideps2 <= (derivationofeps * valideps1)) && (valideps2 < uppereps);
    return Value();
}

Value IfcSameValue(Value value1, Value value2, Value epsilon) {
    Value valideps;
    
    Value defaulteps = 0.000001;
    valideps = nvl(epsilon, defaulteps);
    return ((value1 + valideps) > value2) && (value1 < (value2 + valideps));
    return Value();
}

Value IfcScalarTimesVector(Value scalar, Value vec) {
    Value v;
    Value mag;
    Value result;
    if (Value(!(exists(scalar)) || !(exists(vec))).truthy()) {
        return Value();
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvector")), typeof_(vec))).truthy()) {
            v = express_getattr(vec, "Orientation");
            mag = scalar * (express_getattr(vec, "Magnitude"));
        } else {
            v = vec;
            mag = scalar;
        }
        if (Value(mag < 0.0).truthy()) {
            for (Value i = 1; (Value(i) <= Value(sizeof_(express_getattr(v, "DirectionRatios")))).truthy(); i = i + Value((int64_t)1)) {
                v = ifcapi::express::set_attr(v, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(v, "DirectionRatios"), i - 1, -express_getitem(express_getattr(v, "DirectionRatios"), i - 1)));
            }
            mag = -mag;
        }
        result = IfcVector(IfcNormalise(v),  mag);
    }
    return result;
    return Value();
}

Value IfcSecondProjAxis(Value zaxis, Value xaxis, Value arg) {
    Value v;
    Value temp;
    Value yaxis;
    if (Value(!(exists(arg))).truthy()) {
        v = IfcDirection(Value::make_list({0.0, 1.0, 0.0}));
    } else {
        v = arg;
    }
    temp = IfcScalarTimesVector(IfcDotProduct(v, zaxis), zaxis);
    yaxis = IfcVectorDifference(v, temp);
    temp = IfcScalarTimesVector(IfcDotProduct(v, xaxis), xaxis);
    yaxis = IfcVectorDifference(yaxis, temp);
    yaxis = IfcNormalise(yaxis);
    return express_getattr(yaxis, "Orientation");
    return Value();
}

Value IfcShapeRepresentationTypes(Value reptype, Value items) {
    Value count = 0;
    if (Value((reptype) == Value(std::string("curve2d"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifccurve")), typeof_(temp))) && ((express_getattr(temp, "Dim")) == 2)).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("annotation2d"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_(typeof_(temp) * (Value::make_list({Value(std::string("ifc2x3.ifcpoint")), Value(std::string("ifc2x3.ifccurve")), Value(std::string("ifc2x3.ifcgeometriccurveset")), Value(std::string("ifc2x3.ifcannotationfillarea")), Value(std::string("ifc2x3.ifcdefinedsymbol")), Value(std::string("ifc2x3.ifctextliteral")), Value(std::string("ifc2x3.ifcdraughtingcallout"))})))) == 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("geometricset"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgeometricset")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcpoint")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifccurve")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurface")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("geometriccurveset"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgeometriccurveset")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgeometricset")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcpoint")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifccurve")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
        for (Value i = 1; (Value(i) <= Value(hiindex(items))).truthy(); i = i + Value((int64_t)1)) {
            if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgeometricset")), typeof_(express_getitem(items, i - 1)))).truthy()) {
                if (Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getitem(items, i - 1), "Elements"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) > 0).truthy()) {
                    count = count - 1;
                }
            }
        }
    
    } else if (Value((reptype) == Value(std::string("surfacemodel"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcshellbasedsurfacemodel")), Value(std::string("ifc2x3.ifcfacebasedsurfacemodel")), Value(std::string("ifc2x3.ifcfacetedbrep")), Value(std::string("ifc2x3.ifcfacetedbrepwithvoids"))})) * typeof_(temp))) >= 1).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("solidmodel"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsolidmodel")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("sweptsolid"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsweptareasolid")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("csg"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcbooleanresult")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("clipping"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcbooleanclippingresult")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("advancedsweptsolid"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurfacecurvesweptareasolid")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsweptdisksolid")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("brep"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfacetedbrep")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfacetedbrepwithvoids")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("boundingbox"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcboundingbox")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
        if (Value(sizeof_(items) > 1).truthy()) {
            count = 0;
        }
    
    } else if (Value((reptype) == Value(std::string("sectionedspine"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsectionedspine")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("mappedrepresentation"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcmappeditem")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    } else {
        return Value();
    }
    return count == sizeof_(items);
    return Value();
}

Value IfcTopologyRepresentationTypes(Value reptype, Value items) {
    Value count = 0;
    if (Value((reptype) == Value(std::string("vertex"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvertex")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("edge"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcedge")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("path"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcpath")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("face"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("shell"))).truthy()) {
        count = sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcopenshell")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcclosedshell")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })());
    
    } else if (Value((reptype) == Value(std::string("undefined"))).truthy()) {
        return true;
    } else {
        return Value();
    }
    return count == sizeof_(items);
    return Value();
}

Value IfcUniquePropertyName(Value properties) {
    Value names = Value::make_list({});
    for (Value i = 1; (Value(i) <= Value(hiindex(properties))).truthy(); i = i + Value((int64_t)1)) {
        names = names + (express_getattr(express_getitem(properties, i - 1), "Name"));
    }
    return sizeof_(names) == sizeof_(properties);
    return Value();
}

Value IfcValidCalendarDate(Value date) {
    if (Value(!((1 <= express_getattr(date, "DayComponent") <= 31))).truthy()) {
        return false;
    }
    if (Value((express_getattr(date, "MonthComponent")) == 4).truthy()) {
        return 1 <= express_getattr(date, "DayComponent") <= 30;
    
    } else if (Value((express_getattr(date, "MonthComponent")) == 6).truthy()) {
        return 1 <= express_getattr(date, "DayComponent") <= 30;
    
    } else if (Value((express_getattr(date, "MonthComponent")) == 9).truthy()) {
        return 1 <= express_getattr(date, "DayComponent") <= 30;
    
    } else if (Value((express_getattr(date, "MonthComponent")) == 11).truthy()) {
        return 1 <= express_getattr(date, "DayComponent") <= 30;
    
    } else if (Value((express_getattr(date, "MonthComponent")) == 2).truthy()) {
        if (Value(IfcLeapYear(express_getattr(date, "YearComponent"))).truthy()) {
            return 1 <= express_getattr(date, "DayComponent") <= 29;
        } else {
            return 1 <= express_getattr(date, "DayComponent") <= 28;
        }
    } else {
        return true;
    }
    return Value();
}

Value IfcValidTime(Value time) {
    if (Value(exists(express_getattr(time, "SecondComponent"))).truthy()) {
        return exists(express_getattr(time, "MinuteComponent"));
    } else {
        return true;
    }
    return Value();
}

Value IfcVectorDifference(Value arg1, Value arg2) {
    Value mag1;
    Value vec1;
    Value mag2;
    Value vec2;
    Value ndim;
    Value mag;
    Value res;
    Value result;
    if (Value((!(exists(arg1)) || !(exists(arg2))) || ((express_getattr(arg1, "Dim")) != (express_getattr(arg2, "Dim")))).truthy()) {
        return Value();
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvector")), typeof_(arg1))).truthy()) {
            mag1 = express_getattr(arg1, "Magnitude");
            vec1 = express_getattr(arg1, "Orientation");
        } else {
            mag1 = 1.0;
            vec1 = arg1;
        }
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvector")), typeof_(arg2))).truthy()) {
            mag2 = express_getattr(arg2, "Magnitude");
            vec2 = express_getattr(arg2, "Orientation");
        } else {
            mag2 = 1.0;
            vec2 = arg2;
        }
        vec1 = IfcNormalise(vec1);
        vec2 = IfcNormalise(vec2);
        ndim = sizeof_(express_getattr(vec1, "DirectionRatios"));
        mag = 0.0;
        res = IfcDirection(ifcapi::express::repeat(0.0, ndim));
        for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
            res = ifcapi::express::set_attr(res, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(res, "DirectionRatios"), i - 1, (mag1 * (express_getitem(express_getattr(vec1, "DirectionRatios"), i - 1))) - (mag2 * (express_getitem(express_getattr(vec2, "DirectionRatios"), i - 1)))));
            mag = mag + ((express_getitem(express_getattr(res, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(res, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            result = IfcVector(res,  math_sqrt(mag));
        } else {
            result = IfcVector(vec1,  0.0);
        }
    }
    return result;
    return Value();
}

Value IfcVectorSum(Value arg1, Value arg2) {
    Value mag1;
    Value vec1;
    Value mag2;
    Value vec2;
    Value ndim;
    Value mag;
    Value res;
    Value result;
    if (Value((!(exists(arg1)) || !(exists(arg2))) || ((express_getattr(arg1, "Dim")) != (express_getattr(arg2, "Dim")))).truthy()) {
        return Value();
    } else {
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvector")), typeof_(arg1))).truthy()) {
            mag1 = express_getattr(arg1, "Magnitude");
            vec1 = express_getattr(arg1, "Orientation");
        } else {
            mag1 = 1.0;
            vec1 = arg1;
        }
        if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvector")), typeof_(arg2))).truthy()) {
            mag2 = express_getattr(arg2, "Magnitude");
            vec2 = express_getattr(arg2, "Orientation");
        } else {
            mag2 = 1.0;
            vec2 = arg2;
        }
        vec1 = IfcNormalise(vec1);
        vec2 = IfcNormalise(vec2);
        ndim = sizeof_(express_getattr(vec1, "DirectionRatios"));
        mag = 0.0;
        res = IfcDirection(ifcapi::express::repeat(0.0, ndim));
        for (Value i = 1; (Value(i) <= Value(ndim)).truthy(); i = i + Value((int64_t)1)) {
            res = ifcapi::express::set_attr(res, "DirectionRatios", ifcapi::express::set_index(ifcapi::express::express_getattr(res, "DirectionRatios"), i - 1, (mag1 * (express_getitem(express_getattr(vec1, "DirectionRatios"), i - 1))) + (mag2 * (express_getitem(express_getattr(vec2, "DirectionRatios"), i - 1)))));
            mag = mag + ((express_getitem(express_getattr(res, "DirectionRatios"), i - 1)) * (express_getitem(express_getattr(res, "DirectionRatios"), i - 1)));
        }
        if (Value(mag > 0.0).truthy()) {
            result = IfcVector(res,  math_sqrt(mag));
        } else {
            result = IfcVector(vec1,  0.0);
        }
    }
    return result;
    return Value();
}

// --- type / entity / rule definitions ---
bool IfcBoxAlignment_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("top-left")), Value(std::string("top-middle")), Value(std::string("top-right")), Value(std::string("middle-left")), Value(std::string("center")), Value(std::string("middle-right")), Value(std::string("bottom-left")), Value(std::string("bottom-middle")), Value(std::string("bottom-right"))}))).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_WR1(EntityRef self) {
    if (!Value(-360 <= express_getitem(self, 1 - 1) < 360).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_WR2(EntityRef self) {
    if (!Value(-60 <= express_getitem(self, 2 - 1) < 60).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_WR3(EntityRef self) {
    if (!Value(-60 <= express_getitem(self, 3 - 1) < 60).truthy()) return false;
    return true;
}

bool IfcCompoundPlaneAngleMeasure_WR4(EntityRef self) {
    if (!Value((((express_getitem(self, 1 - 1)) >= 0) && ((express_getitem(self, 2 - 1)) >= 0) && ((express_getitem(self, 3 - 1)) >= 0)) || (((express_getitem(self, 1 - 1)) <= 0) && ((express_getitem(self, 2 - 1)) <= 0) && ((express_getitem(self, 3 - 1)) <= 0))).truthy()) return false;
    return true;
}

bool IfcDaylightSavingHour_WR1(EntityRef self) {
    if (!Value(0 <= self <= 2).truthy()) return false;
    return true;
}

bool IfcDimensionCount_WR1(EntityRef self) {
    if (!Value(0 < self <= 3).truthy()) return false;
    return true;
}

bool IfcFontStyle_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("normal")), Value(std::string("italic")), Value(std::string("oblique"))}))).truthy()) return false;
    return true;
}

bool IfcFontVariant_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("normal")), Value(std::string("small-caps"))}))).truthy()) return false;
    return true;
}

bool IfcFontWeight_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("normal")), Value(std::string("small-caps")), Value(std::string("100")), Value(std::string("200")), Value(std::string("300")), Value(std::string("400")), Value(std::string("500")), Value(std::string("600")), Value(std::string("700")), Value(std::string("800")), Value(std::string("900"))}))).truthy()) return false;
    return true;
}

bool IfcHeatingValueMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcHourInDay_WR1(EntityRef self) {
    if (!Value(0 <= self < 24).truthy()) return false;
    return true;
}

bool IfcMinuteInHour_WR1(EntityRef self) {
    if (!Value(0 <= self <= 59).truthy()) return false;
    return true;
}

bool IfcMonthInYearNumber_WR1(EntityRef self) {
    if (!Value(1 <= self <= 12).truthy()) return false;
    return true;
}

bool IfcNormalisedRatioMeasure_WR1(EntityRef self) {
    if (!Value(0.0 <= self <= 1.0).truthy()) return false;
    return true;
}

bool IfcPHMeasure_WR21(EntityRef self) {
    if (!Value(0.0 <= self <= 14.0).truthy()) return false;
    return true;
}

bool IfcPositiveLengthMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcPositivePlaneAngleMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcPositiveRatioMeasure_WR1(EntityRef self) {
    if (!Value(self > 0.).truthy()) return false;
    return true;
}

bool IfcSecondInMinute_WR1(EntityRef self) {
    if (!Value(0. <= self < 60.).truthy()) return false;
    return true;
}

bool IfcSpecularRoughness_WR1(EntityRef self) {
    if (!Value(0.0 <= self <= 1.0).truthy()) return false;
    return true;
}

bool IfcTextAlignment_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("left")), Value(std::string("right")), Value(std::string("center")), Value(std::string("justify"))}))).truthy()) return false;
    return true;
}

bool IfcTextDecoration_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("none")), Value(std::string("underline")), Value(std::string("overline")), Value(std::string("line-through")), Value(std::string("blink"))}))).truthy()) return false;
    return true;
}

bool IfcTextTransformation_WR1(EntityRef self) {
    if (!Value(ifcapi::express::express_in(self, Value::make_list({Value(std::string("capitalize")), Value(std::string("uppercase")), Value(std::string("lowercase")), Value(std::string("none"))}))).truthy()) return false;
    return true;
}

bool Ifc2DCompositeCurve_WR1(EntityRef self) {
    if (!Value(express_getattr(self, "ClosedCurve")).truthy()) return false;
    return true;
}

bool Ifc2DCompositeCurve_WR2(EntityRef self) {
    if (!Value((express_getattr(self, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcActorRole_WR1(EntityRef self) {
    Value role = express_getattr(self, "Role");
    if (!Value((role != Value(std::string("USERDEFINED"))) || ((role == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedRole"))))).truthy()) return false;
    return true;
}

bool IfcAddress_WR1(EntityRef self) {
    Value purpose = express_getattr(self, "Purpose");
    if (!Value(!(exists(purpose)) || ((purpose != Value(std::string("USERDEFINED"))) || ((purpose == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedPurpose")))))).truthy()) return false;
    return true;
}

bool IfcAirTerminalBoxType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAirTerminalType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAirToAirHeatRecoveryType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcAnnotationCurveOccurrence_WR31(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Item"))))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifccurve")), typeof_(express_getattr(self, "Item"))))).truthy()) return false;
    return true;
}

bool IfcAnnotationFillAreaOccurrence_WR31(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Item"))))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcannotationfillarea")), typeof_(express_getattr(self, "Item"))))).truthy()) return false;
    return true;
}

bool IfcAnnotationSurface_WR01(EntityRef self) {
    Value item = express_getattr(self, "Item");
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcsurface")), Value(std::string("ifc2x3.ifcshellbasedsurfacemodel")), Value(std::string("ifc2x3.ifcfacebasedsurfacemodel")), Value(std::string("ifc2x3.ifcsolidmodel")), Value(std::string("ifc2x3.ifcbooleanresult")), Value(std::string("ifc2x3.ifccsgprimitive3d"))})) * typeof_(item))) >= 1).truthy()) return false;
    return true;
}

bool IfcAnnotationSurfaceOccurrence_WR31(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Item"))))) || ((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcsurface")), Value(std::string("ifc2x3.ifcfacebasedsurfacemodel")), Value(std::string("ifc2x3.ifcshellbasedsurfacemodel")), Value(std::string("ifc2x3.ifcsolidmodel"))})) * (typeof_(express_getattr(self, "Item"))))) > 0)).truthy()) return false;
    return true;
}

bool IfcAnnotationSymbolOccurrence_WR31(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Item"))))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdefinedsymbol")), typeof_(express_getattr(self, "Item"))))).truthy()) return false;
    return true;
}

bool IfcAnnotationTextOccurrence_WR31(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Item"))))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifctextliteral")), typeof_(express_getattr(self, "Item"))))).truthy()) return false;
    return true;
}

bool IfcAppliedValue_WR1(EntityRef self) {
    Value appliedvalue = express_getattr(self, "AppliedValue");
    Value valueofcomponents = express_getattr(self, "ValueOfComponents");
    if (!Value(exists(appliedvalue) || exists(valueofcomponents)).truthy()) return false;
    return true;
}

bool IfcArbitraryClosedProfileDef_WR1(EntityRef self) {
    Value outercurve = express_getattr(self, "OuterCurve");
    if (!Value((express_getattr(outercurve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcArbitraryClosedProfileDef_WR2(EntityRef self) {
    Value outercurve = express_getattr(self, "OuterCurve");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcline")), typeof_(outercurve))))).truthy()) return false;
    return true;
}

bool IfcArbitraryClosedProfileDef_WR3(EntityRef self) {
    Value outercurve = express_getattr(self, "OuterCurve");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcoffsetcurve2d")), typeof_(outercurve))))).truthy()) return false;
    return true;
}

bool IfcArbitraryOpenProfileDef_WR11(EntityRef self) {
    if (!Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifccenterlineprofiledef")), typeof_(self))) || ((express_getattr(self, "ProfileType")) == Value(std::string("CURVE")))).truthy()) return false;
    return true;
}

bool IfcArbitraryOpenProfileDef_WR12(EntityRef self) {
    Value curve = express_getattr(self, "Curve");
    if (!Value((express_getattr(curve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcArbitraryProfileDefWithVoids_WR1(EntityRef self) {
    if (!Value((express_getattr(self, "ProfileType")) == area).truthy()) return false;
    return true;
}

bool IfcArbitraryProfileDefWithVoids_WR2(EntityRef self) {
    Value innercurves = express_getattr(self, "InnerCurves");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(innercurves)) { if (Value((express_getattr(temp, "Dim")) != 2).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcArbitraryProfileDefWithVoids_WR3(EntityRef self) {
    Value innercurves = express_getattr(self, "InnerCurves");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(innercurves)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcline")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcAsset_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getattr(self, "IsGroupedBy"), "RelatedObjects"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcelement")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcAxis1Placement_WR1(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value(!(exists(axis)) || ((express_getattr(axis, "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcAxis1Placement_WR2(EntityRef self) {
    if (!Value((express_getattr(express_getattr(self, "Location"), "Dim")) == 3).truthy()) return false;
    return true;
}

Value calc_IfcAxis1Placement_Z(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    return nvl(IfcNormalise(axis), IfcDirection(Value::make_list({0.0, 0.0, 1.0})));
}

bool IfcAxis2Placement2D_WR1(EntityRef self) {
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!(exists(refdirection)) || ((express_getattr(refdirection, "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement2D_WR2(EntityRef self) {
    if (!Value((express_getattr(express_getattr(self, "Location"), "Dim")) == 2).truthy()) return false;
    return true;
}

Value calc_IfcAxis2Placement2D_P(EntityRef self) {
    Value refdirection = express_getattr(self, "RefDirection");
    return IfcBuild2Axes(refdirection);
}

bool IfcAxis2Placement3D_WR1(EntityRef self) {
    if (!Value((express_getattr(express_getattr(self, "Location"), "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_WR2(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value(!(exists(axis)) || ((express_getattr(axis, "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_WR3(EntityRef self) {
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!(exists(refdirection)) || ((express_getattr(refdirection, "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_WR4(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!(exists(axis)) || !(exists(refdirection)) || ((express_getattr(IfcCrossProduct(axis, refdirection), "Magnitude")) > 0.0)).truthy()) return false;
    return true;
}

bool IfcAxis2Placement3D_WR5(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    Value refdirection = express_getattr(self, "RefDirection");
    if (!Value(!((exists(axis) ^ exists(refdirection)))).truthy()) return false;
    return true;
}

Value calc_IfcAxis2Placement3D_P(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    Value refdirection = express_getattr(self, "RefDirection");
    return IfcBuildAxes(axis, refdirection);
}

bool IfcBSplineCurve_WR41(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(controlpointslist)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(controlpointslist, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcBSplineCurve_ControlPoints(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    Value upperindexoncontrolpoints = express_getattr(self, "UpperIndexOnControlPoints");
    return IfcListToArray(controlpointslist, 0, upperindexoncontrolpoints);
}

Value calc_IfcBSplineCurve_UpperIndexOnControlPoints(EntityRef self) {
    Value controlpointslist = express_getattr(self, "ControlPointsList");
    return sizeof_(controlpointslist) - 1;
}

bool IfcBlobTexture_WR11(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "RasterFormat"), Value::make_list({Value(std::string("bmp")), Value(std::string("jpg")), Value(std::string("gif")), Value(std::string("png"))}))).truthy()) return false;
    return true;
}

bool IfcBoilerType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcBooleanClippingResult_WR1(EntityRef self) {
    Value firstoperand = express_getattr(self, "FirstOperand");
    if (!Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsweptareasolid")), typeof_(firstoperand))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcbooleanclippingresult")), typeof_(firstoperand)))).truthy()) return false;
    return true;
}

bool IfcBooleanClippingResult_WR2(EntityRef self) {
    Value secondoperand = express_getattr(self, "SecondOperand");
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifchalfspacesolid")), typeof_(secondoperand))).truthy()) return false;
    return true;
}

bool IfcBooleanClippingResult_WR3(EntityRef self) {
    Value operator_ = express_getattr(self, "Operator");
    if (!Value(operator_ == difference).truthy()) return false;
    return true;
}

bool IfcBooleanResult_WR1(EntityRef self) {
    Value firstoperand = express_getattr(self, "FirstOperand");
    Value secondoperand = express_getattr(self, "SecondOperand");
    if (!Value((express_getattr(firstoperand, "Dim")) == (express_getattr(secondoperand, "Dim"))).truthy()) return false;
    return true;
}

Value calc_IfcBooleanResult_Dim(EntityRef self) {
    Value firstoperand = express_getattr(self, "FirstOperand");
    return express_getattr(firstoperand, "Dim");
}

Value calc_IfcBoundingBox_Dim(EntityRef self) {
    return 3;
}

bool IfcBoxedHalfSpace_WR1(EntityRef self) {
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifccurveboundedplane")), typeof_(express_getattr(self, "BaseSurface")))))).truthy()) return false;
    return true;
}

bool IfcBuildingElementProxy_WR1(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcCShapeProfileDef_WR1(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value girth = express_getattr(self, "Girth");
    if (!Value(girth < (depth / 2.)).truthy()) return false;
    return true;
}

bool IfcCShapeProfileDef_WR2(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value width = express_getattr(self, "Width");
    Value internalfilletradius = express_getattr(self, "InternalFilletRadius");
    if (!Value(!(exists(internalfilletradius)) || ((internalfilletradius <= (width / 2.)) && (internalfilletradius <= (depth / 2.)))).truthy()) return false;
    return true;
}

bool IfcCShapeProfileDef_WR3(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value width = express_getattr(self, "Width");
    Value wallthickness = express_getattr(self, "WallThickness");
    if (!Value((wallthickness < (width / 2.)) && (wallthickness < (depth / 2.))).truthy()) return false;
    return true;
}

bool IfcCableCarrierFittingType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCalendarDate_WR21(EntityRef self) {
    if (!Value(IfcValidCalendarDate(self)).truthy()) return false;
    return true;
}

bool IfcCartesianPoint_WR1(EntityRef self) {
    Value coordinates = express_getattr(self, "Coordinates");
    if (!Value(hiindex(coordinates) >= 2).truthy()) return false;
    return true;
}

Value calc_IfcCartesianPoint_Dim(EntityRef self) {
    Value coordinates = express_getattr(self, "Coordinates");
    return hiindex(coordinates);
}

bool IfcCartesianTransformationOperator_WR1(EntityRef self) {
    Value scl = express_getattr(self, "Scl");
    if (!Value(scl > 0.0).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator_Scl(EntityRef self) {
    Value scale = express_getattr(self, "Scale");
    return nvl(scale, 1.0);
}

Value calc_IfcCartesianTransformationOperator_Dim(EntityRef self) {
    Value localorigin = express_getattr(self, "LocalOrigin");
    return express_getattr(localorigin, "Dim");
}

bool IfcCartesianTransformationOperator2D_WR1(EntityRef self) {
    if (!Value((express_getattr(self, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator2D_WR2(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis1"))))) || ((express_getattr(express_getattr(self, "Axis1"), "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator2D_WR3(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis2"))))) || ((express_getattr(express_getattr(self, "Axis2"), "Dim")) == 2)).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator2D_U(EntityRef self) {
    return IfcBaseAxis(2, express_getattr(self, "Axis1"), express_getattr(self, "Axis2"), Value());
}

bool IfcCartesianTransformationOperator2DnonUniform_WR1(EntityRef self) {
    Value scl2 = express_getattr(self, "Scl2");
    if (!Value(scl2 > 0.0).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator2DnonUniform_Scl2(EntityRef self) {
    Value scale2 = express_getattr(self, "Scale2");
    return nvl(scale2, express_getattr(self, "Scl"));
}

bool IfcCartesianTransformationOperator3D_WR1(EntityRef self) {
    if (!Value((express_getattr(self, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3D_WR2(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis1"))))) || ((express_getattr(express_getattr(self, "Axis1"), "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3D_WR3(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Axis2"))))) || ((express_getattr(express_getattr(self, "Axis2"), "Dim")) == 3)).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3D_WR4(EntityRef self) {
    Value axis3 = express_getattr(self, "Axis3");
    if (!Value(!(exists(axis3)) || ((express_getattr(axis3, "Dim")) == 3)).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator3D_U(EntityRef self) {
    Value axis3 = express_getattr(self, "Axis3");
    return IfcBaseAxis(3, express_getattr(self, "Axis1"), express_getattr(self, "Axis2"), axis3);
}

bool IfcCartesianTransformationOperator3DnonUniform_WR1(EntityRef self) {
    Value scl2 = express_getattr(self, "Scl2");
    if (!Value(scl2 > 0.0).truthy()) return false;
    return true;
}

bool IfcCartesianTransformationOperator3DnonUniform_WR2(EntityRef self) {
    Value scl3 = express_getattr(self, "Scl3");
    if (!Value(scl3 > 0.0).truthy()) return false;
    return true;
}

Value calc_IfcCartesianTransformationOperator3DnonUniform_Scl2(EntityRef self) {
    Value scale2 = express_getattr(self, "Scale2");
    return nvl(scale2, express_getattr(self, "Scl"));
}

Value calc_IfcCartesianTransformationOperator3DnonUniform_Scl3(EntityRef self) {
    Value scale3 = express_getattr(self, "Scale3");
    return nvl(scale3, express_getattr(self, "Scl"));
}

bool IfcChillerType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCircleHollowProfileDef_WR1(EntityRef self) {
    Value wallthickness = express_getattr(self, "WallThickness");
    if (!Value(wallthickness < (express_getattr(self, "Radius"))).truthy()) return false;
    return true;
}

bool IfcCoilType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcComplexProperty_WR21(EntityRef self) {
    Value hasproperties = express_getattr(self, "HasProperties");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(hasproperties)) { if (Value(self == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcComplexProperty_WR22(EntityRef self) {
    Value hasproperties = express_getattr(self, "HasProperties");
    if (!Value(IfcUniquePropertyName(hasproperties)).truthy()) return false;
    return true;
}

bool IfcCompositeCurve_WR41(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    Value closedcurve = express_getattr(self, "ClosedCurve");
    if (!Value((!(closedcurve) && ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(segments)) { if (Value((express_getattr(temp, "Transition")) == discontinuous).truthy()) __r.append(temp); } return __r; })())) == 1)) || (closedcurve && ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(segments)) { if (Value((express_getattr(temp, "Transition")) == discontinuous).truthy()) __r.append(temp); } return __r; })())) == 0))).truthy()) return false;
    return true;
}

bool IfcCompositeCurve_WR42(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(segments)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(segments, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcCompositeCurve_NSegments(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    return sizeof_(segments);
}

Value calc_IfcCompositeCurve_ClosedCurve(EntityRef self) {
    Value segments = express_getattr(self, "Segments");
    Value nsegments = express_getattr(self, "NSegments");
    return (express_getattr(express_getitem(segments, nsegments - 1), "Transition")) != discontinuous;
}

bool IfcCompositeCurveSegment_WR1(EntityRef self) {
    Value parentcurve = express_getattr(self, "ParentCurve");
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcboundedcurve")), typeof_(parentcurve))).truthy()) return false;
    return true;
}

Value calc_IfcCompositeCurveSegment_Dim(EntityRef self) {
    Value parentcurve = express_getattr(self, "ParentCurve");
    return express_getattr(parentcurve, "Dim");
}

bool IfcCompositeProfileDef_WR1(EntityRef self) {
    Value profiles = express_getattr(self, "Profiles");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(profiles)) { if (Value((express_getattr(temp, "ProfileType")) != (express_getattr(express_getitem(profiles, 1 - 1), "ProfileType"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcCompositeProfileDef_WR2(EntityRef self) {
    Value profiles = express_getattr(self, "Profiles");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(profiles)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifccompositeprofiledef")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcCompressorType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCondenserType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcConditionCriterion_WR1(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcConstraint_WR11(EntityRef self) {
    Value constraintgrade = express_getattr(self, "ConstraintGrade");
    if (!Value((constraintgrade != Value(std::string("USERDEFINED"))) || ((constraintgrade == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedGrade"))))).truthy()) return false;
    return true;
}

bool IfcConstraintAggregationRelationship_WR11(EntityRef self) {
    Value relatingconstraint = express_getattr(self, "RelatingConstraint");
    Value relatedconstraints = express_getattr(self, "RelatedConstraints");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedconstraints)) { if (Value(temp == relatingconstraint).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcConstraintRelationship_WR11(EntityRef self) {
    Value relatingconstraint = express_getattr(self, "RelatingConstraint");
    Value relatedconstraints = express_getattr(self, "RelatedConstraints");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedconstraints)) { if (Value(temp == relatingconstraint).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcConstructionMaterialResource_WR1(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "ResourceOf"))) <= 1).truthy()) return false;
    return true;
}

bool IfcConstructionMaterialResource_WR2(EntityRef self) {
    if (!Value((!((exists(express_getitem(express_getattr(self, "ResourceOf"), 1 - 1))))) || ((express_getattr(express_getitem(express_getattr(self, "ResourceOf"), 1 - 1), "RelatedObjectsType")) == Value(std::string("PRODUCT")))).truthy()) return false;
    return true;
}

bool IfcConstructionProductResource_WR1(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "ResourceOf"))) <= 1).truthy()) return false;
    return true;
}

bool IfcConstructionProductResource_WR2(EntityRef self) {
    if (!Value((!((exists(express_getitem(express_getattr(self, "ResourceOf"), 1 - 1))))) || ((express_getattr(express_getitem(express_getattr(self, "ResourceOf"), 1 - 1), "RelatedObjectsType")) == Value(std::string("PRODUCT")))).truthy()) return false;
    return true;
}

bool IfcCooledBeamType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCoolingTowerType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcCovering_WR61(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

Value calc_IfcCsgPrimitive3D_Dim(EntityRef self) {
    return 3;
}

Value calc_IfcCurve_Dim(EntityRef self) {
    return IfcCurveDim(self);
}

Value calc_IfcCurveBoundedPlane_Dim(EntityRef self) {
    Value basissurface = express_getattr(self, "BasisSurface");
    return express_getattr(basissurface, "Dim");
}

bool IfcCurveStyle_WR11(EntityRef self) {
    Value curvewidth = express_getattr(self, "CurveWidth");
    if (!Value(!(exists(curvewidth)) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcpositivelengthmeasure")), typeof_(curvewidth))) || ((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdescriptivemeasure")), typeof_(curvewidth))) && (curvewidth == Value(std::string("bylayer"))))).truthy()) return false;
    return true;
}

bool IfcCurveStyleFontPattern_WR01(EntityRef self) {
    Value visiblesegmentlength = express_getattr(self, "VisibleSegmentLength");
    if (!Value(visiblesegmentlength >= 0.).truthy()) return false;
    return true;
}

bool IfcDamperType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDerivedProfileDef_WR1(EntityRef self) {
    Value parentprofile = express_getattr(self, "ParentProfile");
    if (!Value((express_getattr(self, "ProfileType")) == (express_getattr(parentprofile, "ProfileType"))).truthy()) return false;
    return true;
}

bool IfcDerivedUnit_WR1(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    if (!Value((sizeof_(elements) > 1) || ((sizeof_(elements) == 1) && ((express_getattr(express_getitem(elements, 1 - 1), "Exponent")) != 1))).truthy()) return false;
    return true;
}

bool IfcDerivedUnit_WR2(EntityRef self) {
    Value unittype = express_getattr(self, "UnitType");
    if (!Value((unittype != Value(std::string("USERDEFINED"))) || ((unittype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedType"))))).truthy()) return false;
    return true;
}

Value calc_IfcDerivedUnit_Dimensions(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    return IfcDeriveDimensionalExponents(elements);
}

bool IfcDimensionCalloutRelationship_WR11(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("primary")), Value(std::string("secondary"))}))).truthy()) return false;
    return true;
}

bool IfcDimensionCalloutRelationship_WR12(EntityRef self) {
    if (!Value((sizeof_((typeof_(express_getattr(self, "RelatingDraughtingCallout"))) * (Value::make_list({Value(std::string("ifc2x3.ifcangulardimension")), Value(std::string("ifc2x3.ifcdiameterdimension")), Value(std::string("ifc2x3.ifclineardimension")), Value(std::string("ifc2x3.ifcradiusdimension"))})))) == 1).truthy()) return false;
    return true;
}

bool IfcDimensionCalloutRelationship_WR13(EntityRef self) {
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdimensioncurvedirectedcallout")), typeof_(express_getattr(self, "RelatedDraughtingCallout")))))).truthy()) return false;
    return true;
}

bool IfcDimensionCurve_WR51(EntityRef self) {
    if (!Value((sizeof_(usedin(self, Value(std::string("ifc2x3.ifcdraughtingcallout.contents"))))) >= 1).truthy()) return false;
    return true;
}

bool IfcDimensionCurve_WR52(EntityRef self) {
    if (!Value(((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& dct1 : ifcapi::express::iter(usedin(self, Value(std::string("ifc2x3.")) + Value(std::string("ifcterminatorsymbol.annotatedcurve"))))) { if (Value((express_getattr(dct1, "Role")) == Value(std::string("ORIGIN"))).truthy()) __r.append(dct1); } return __r; })())) <= 1) && ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& dct2 : ifcapi::express::iter(usedin(self, Value(std::string("ifc2x3.")) + Value(std::string("ifcterminatorsymbol.annotatedcurve"))))) { if (Value((express_getattr(dct2, "Role")) == Value(std::string("TARGET"))).truthy()) __r.append(dct2); } return __r; })())) <= 1)).truthy()) return false;
    return true;
}

bool IfcDimensionCurve_WR53(EntityRef self) {
    Value annotatedbysymbols = express_getattr(self, "AnnotatedBySymbols");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& dct : ifcapi::express::iter(annotatedbysymbols)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdimensioncurveterminator")), typeof_(dct))))).truthy()) __r.append(dct); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcDimensionCurveDirectedCallout_WR41(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& dc : ifcapi::express::iter(express_getattr(self, "Contents"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdimensioncurve")), typeof_(dc))).truthy()) __r.append(dc); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcDimensionCurveDirectedCallout_WR42(EntityRef self) {
    Value contents = express_getattr(self, "Contents");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& dc : ifcapi::express::iter(express_getattr(self, "contents"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcprojectioncurve")), typeof_(dc))).truthy()) __r.append(dc); } return __r; })())) <= 2).truthy()) return false;
    return true;
}

bool IfcDimensionCurveTerminator_WR61(EntityRef self) {
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdimensioncurve")), typeof_(express_getattr(self, "AnnotatedCurve")))).truthy()) return false;
    return true;
}

bool IfcDimensionPair_WR11(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("chained")), Value(std::string("parallel"))}))).truthy()) return false;
    return true;
}

bool IfcDimensionPair_WR12(EntityRef self) {
    if (!Value((sizeof_((typeof_(express_getattr(self, "RelatingDraughtingCallout"))) * (Value::make_list({Value(std::string("ifc2x3.ifcangulardimension")), Value(std::string("ifc2x3.ifcdiameterdimension")), Value(std::string("ifc2x3.ifclineardimension")), Value(std::string("ifc2x3.ifcradiusdimension"))})))) == 1).truthy()) return false;
    return true;
}

bool IfcDimensionPair_WR13(EntityRef self) {
    if (!Value((sizeof_((typeof_(express_getattr(self, "RelatedDraughtingCallout"))) * (Value::make_list({Value(std::string("ifc2x3.ifcangulardimension")), Value(std::string("ifc2x3.ifcdiameterdimension")), Value(std::string("ifc2x3.ifclineardimension")), Value(std::string("ifc2x3.ifcradiusdimension"))})))) == 1).truthy()) return false;
    return true;
}

Value calc_IfcDirection_Dim(EntityRef self) {
    Value directionratios = express_getattr(self, "DirectionRatios");
    return hiindex(directionratios);
}

bool IfcDocumentElectronicFormat_WR1(EntityRef self) {
    Value fileextension = express_getattr(self, "FileExtension");
    Value mimecontenttype = express_getattr(self, "MimeContentType");
    if (!Value(exists(fileextension) || exists(mimecontenttype)).truthy()) return false;
    return true;
}

bool IfcDocumentReference_WR1(EntityRef self) {
    Value name = express_getattr(self, "Name");
    Value referencetodocument = express_getattr(self, "ReferenceToDocument");
    if (!Value(exists(name) ^ (exists(express_getitem(referencetodocument, 1 - 1)))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR31(EntityRef self) {
    Value liningdepth = express_getattr(self, "LiningDepth");
    Value liningthickness = express_getattr(self, "LiningThickness");
    if (!Value(!((!(exists(liningdepth)) && exists(liningthickness)))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR32(EntityRef self) {
    Value thresholddepth = express_getattr(self, "ThresholdDepth");
    Value thresholdthickness = express_getattr(self, "ThresholdThickness");
    if (!Value(!((!(exists(thresholddepth)) && exists(thresholdthickness)))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR33(EntityRef self) {
    Value transomthickness = express_getattr(self, "TransomThickness");
    Value transomoffset = express_getattr(self, "TransomOffset");
    if (!Value((exists(transomoffset) && exists(transomthickness)) ^ (!(exists(transomoffset)) && !(exists(transomthickness)))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR34(EntityRef self) {
    Value casingthickness = express_getattr(self, "CasingThickness");
    Value casingdepth = express_getattr(self, "CasingDepth");
    if (!Value((exists(casingdepth) && exists(casingthickness)) ^ (!(exists(casingdepth)) && !(exists(casingthickness)))).truthy()) return false;
    return true;
}

bool IfcDoorLiningProperties_WR35(EntityRef self) {
    if (!Value((exists(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))) && (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdoorstyle")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))))).truthy()) return false;
    return true;
}

bool IfcDoorPanelProperties_WR31(EntityRef self) {
    if (!Value((exists(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))) && (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcdoorstyle")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))))).truthy()) return false;
    return true;
}

bool IfcDraughtingPreDefinedColour_WR31(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("black")), Value(std::string("red")), Value(std::string("green")), Value(std::string("blue")), Value(std::string("yellow")), Value(std::string("magenta")), Value(std::string("cyan")), Value(std::string("white")), Value(std::string("bylayer"))}))).truthy()) return false;
    return true;
}

bool IfcDraughtingPreDefinedCurveFont_WR31(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("continuous")), Value(std::string("chain")), Value(std::string("chaindoubledash")), Value(std::string("dashed")), Value(std::string("dotted")), Value(std::string("bylayer"))}))).truthy()) return false;
    return true;
}

bool IfcDraughtingPreDefinedTextFont_WR31(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("iso3098-1fonta")), Value(std::string("iso3098-1fontb"))}))).truthy()) return false;
    return true;
}

bool IfcDuctFittingType_WR2(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSegmentType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcDuctSilencerType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcEdgeLoop_WR1(EntityRef self) {
    Value edgelist = express_getattr(self, "EdgeList");
    Value ne = express_getattr(self, "Ne");
    if (!Value((express_getattr(express_getitem(edgelist, 1 - 1), "EdgeStart")) == (express_getattr(express_getitem(edgelist, ne - 1), "EdgeEnd"))).truthy()) return false;
    return true;
}

bool IfcEdgeLoop_WR2(EntityRef self) {
    if (!Value(IfcLoopHeadToTail(self)).truthy()) return false;
    return true;
}

Value calc_IfcEdgeLoop_Ne(EntityRef self) {
    Value edgelist = express_getattr(self, "EdgeList");
    return sizeof_(edgelist);
}

bool IfcElectricDistributionPoint_WR31(EntityRef self) {
    Value distributionpointfunction = express_getattr(self, "DistributionPointFunction");
    if (!Value((distributionpointfunction != Value(std::string("USERDEFINED"))) || ((distributionpointfunction == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedFunction"))))).truthy()) return false;
    return true;
}

bool IfcElementAssembly_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

Value calc_IfcElementarySurface_Dim(EntityRef self) {
    Value position = express_getattr(self, "Position");
    return express_getattr(position, "Dim");
}

bool IfcEnvironmentalImpactValue_WR1(EntityRef self) {
    Value category = express_getattr(self, "Category");
    if (!Value((category != Value(std::string("USERDEFINED"))) || ((category == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedCategory"))))).truthy()) return false;
    return true;
}

bool IfcEvaporativeCoolerType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcEvaporatorType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcExternalReference_WR1(EntityRef self) {
    Value location = express_getattr(self, "Location");
    Value itemreference = express_getattr(self, "ItemReference");
    Value name = express_getattr(self, "Name");
    if (!Value(exists(itemreference) || exists(location) || exists(name)).truthy()) return false;
    return true;
}

bool IfcExtrudedAreaSolid_WR31(EntityRef self) {
    if (!Value((IfcDotProduct(IfcDirection(Value::make_list({0.0, 0.0, 1.0})), express_getattr(self, "ExtrudedDirection"))) != 0.0).truthy()) return false;
    return true;
}

bool IfcFace_WR1(EntityRef self) {
    Value bounds = express_getattr(self, "Bounds");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(bounds)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfaceouterbound")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

Value calc_IfcFaceBasedSurfaceModel_Dim(EntityRef self) {
    return 3;
}

bool IfcFanType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFillAreaStyle_WR11(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "FillStyles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifccolour")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcFillAreaStyle_WR12(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "FillStyles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcexternallydefinedhatchstyle")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcFillAreaStyle_WR13(EntityRef self) {
    if (!Value(IfcCorrectFillAreaStyle(express_getattr(self, "FillStyles"))).truthy()) return false;
    return true;
}

bool IfcFillAreaStyleHatching_WR21(EntityRef self) {
    Value startofnexthatchline = express_getattr(self, "StartOfNextHatchLine");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifctwodirectionrepeatfactor")), typeof_(startofnexthatchline))))).truthy()) return false;
    return true;
}

bool IfcFillAreaStyleHatching_WR22(EntityRef self) {
    Value patternstart = express_getattr(self, "PatternStart");
    if (!Value(!(exists(patternstart)) || ((express_getattr(patternstart, "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcFillAreaStyleHatching_WR23(EntityRef self) {
    Value pointofreferencehatchline = express_getattr(self, "PointOfReferenceHatchLine");
    if (!Value(!(exists(pointofreferencehatchline)) || ((express_getattr(pointofreferencehatchline, "Dim")) == 2)).truthy()) return false;
    return true;
}

bool IfcFilterType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFlowMeterType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcFooting_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcGasTerminalType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcGeneralProfileProperties_WR1(EntityRef self) {
    Value crosssectionarea = express_getattr(self, "CrossSectionArea");
    if (!Value(!(exists(crosssectionarea)) || (crosssectionarea > 0.)).truthy()) return false;
    return true;
}

bool IfcGeometricCurveSet_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Elements"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurface")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcGeometricRepresentationSubContext_WR31(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgeometricrepresentationsubcontext")), typeof_(parentcontext))))).truthy()) return false;
    return true;
}

bool IfcGeometricRepresentationSubContext_WR32(EntityRef self) {
    Value targetview = express_getattr(self, "TargetView");
    Value userdefinedtargetview = express_getattr(self, "UserDefinedTargetView");
    if (!Value((targetview != Value(std::string("USERDEFINED"))) || ((targetview == Value(std::string("USERDEFINED"))) && exists(userdefinedtargetview))).truthy()) return false;
    return true;
}

Value calc_IfcGeometricRepresentationSubContext_WorldCoordinateSystem(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return express_getattr(parentcontext, "WorldCoordinateSystem");
}

Value calc_IfcGeometricRepresentationSubContext_CoordinateSpaceDimension(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return express_getattr(parentcontext, "CoordinateSpaceDimension");
}

Value calc_IfcGeometricRepresentationSubContext_TrueNorth(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return nvl(express_getattr(parentcontext, "TrueNorth"), express_getitem(express_getattr(express_getattr(self, "WorldCoordinateSystem"), "P"), 2 - 1));
}

Value calc_IfcGeometricRepresentationSubContext_Precision(EntityRef self) {
    Value parentcontext = express_getattr(self, "ParentContext");
    return nvl(express_getattr(parentcontext, "Precision"), 1);
}

bool IfcGeometricSet_WR21(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(elements)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(elements, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcGeometricSet_Dim(EntityRef self) {
    Value elements = express_getattr(self, "Elements");
    return express_getattr(express_getitem(elements, 1 - 1), "Dim");
}

bool IfcGrid_WR41(EntityRef self) {
    if (!Value(exists(express_getattr(self, "ObjectPlacement"))).truthy()) return false;
    return true;
}

bool IfcGridAxis_WR1(EntityRef self) {
    Value axiscurve = express_getattr(self, "AxisCurve");
    if (!Value((express_getattr(axiscurve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcGridAxis_WR2(EntityRef self) {
    Value partofw = express_getattr(self, "PartOfW");
    Value partofv = express_getattr(self, "PartOfV");
    Value partofu = express_getattr(self, "PartOfU");
    if (!Value((sizeof_(partofu) == 1) ^ (sizeof_(partofv) == 1) ^ (sizeof_(partofw) == 1)).truthy()) return false;
    return true;
}

Value calc_IfcHalfSpaceSolid_Dim(EntityRef self) {
    return 3;
}

bool IfcHeatExchangerType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcHumidifierType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcIShapeProfileDef_WR1(EntityRef self) {
    Value overalldepth = express_getattr(self, "OverallDepth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value(flangethickness < (overalldepth / 2.)).truthy()) return false;
    return true;
}

bool IfcIShapeProfileDef_WR2(EntityRef self) {
    Value overallwidth = express_getattr(self, "OverallWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    if (!Value(webthickness < overallwidth).truthy()) return false;
    return true;
}

bool IfcIShapeProfileDef_WR3(EntityRef self) {
    Value overallwidth = express_getattr(self, "OverallWidth");
    Value overalldepth = express_getattr(self, "OverallDepth");
    Value webthickness = express_getattr(self, "WebThickness");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    Value filletradius = express_getattr(self, "FilletRadius");
    if (!Value(!(exists(filletradius)) || ((filletradius <= ((overallwidth - webthickness) / 2.)) && (filletradius <= ((overalldepth - (2. * flangethickness)) / 2.)))).truthy()) return false;
    return true;
}

bool IfcInventory_WR41(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getattr(self, "IsGroupedBy"), "RelatedObjects"))) { if (Value(!(((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcspace")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcasset")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfurnishingelement")), typeof_(temp)))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcLShapeProfileDef_WR21(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value thickness = express_getattr(self, "Thickness");
    if (!Value(thickness < depth).truthy()) return false;
    return true;
}

bool IfcLShapeProfileDef_WR22(EntityRef self) {
    Value width = express_getattr(self, "Width");
    Value thickness = express_getattr(self, "Thickness");
    if (!Value(!(exists(width)) || (thickness < width)).truthy()) return false;
    return true;
}

bool IfcLine_WR1(EntityRef self) {
    Value pnt = express_getattr(self, "Pnt");
    Value dir = express_getattr(self, "Dir");
    if (!Value((express_getattr(dir, "Dim")) == (express_getattr(pnt, "Dim"))).truthy()) return false;
    return true;
}

bool IfcLocalPlacement_WR21(EntityRef self) {
    Value placementrelto = express_getattr(self, "PlacementRelTo");
    Value relativeplacement = express_getattr(self, "RelativePlacement");
    if (!Value(IfcCorrectLocalPlacement(relativeplacement, placementrelto)).truthy()) return false;
    return true;
}

bool IfcLocalTime_WR21(EntityRef self) {
    if (!Value(IfcValidTime(self)).truthy()) return false;
    return true;
}

bool IfcMaterialDefinitionRepresentation_WR11(EntityRef self) {
    Value representations = express_getattr(self, "Representations");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(representations)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcstyledrepresentation")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcMaterialLayerSet_TotalThickness(EntityRef self) {
    return IfcMlsTotalThickness(self);
}

bool IfcMechanicalMaterialProperties_WR21(EntityRef self) {
    Value youngmodulus = express_getattr(self, "YoungModulus");
    if (!Value(!(exists(youngmodulus)) || (youngmodulus >= 0.0)).truthy()) return false;
    return true;
}

bool IfcMechanicalMaterialProperties_WR22(EntityRef self) {
    Value shearmodulus = express_getattr(self, "ShearModulus");
    if (!Value(!(exists(shearmodulus)) || (shearmodulus >= 0.0)).truthy()) return false;
    return true;
}

bool IfcMechanicalSteelMaterialProperties_WR31(EntityRef self) {
    Value yieldstress = express_getattr(self, "YieldStress");
    if (!Value(!(exists(yieldstress)) || (yieldstress >= 0.)).truthy()) return false;
    return true;
}

bool IfcMechanicalSteelMaterialProperties_WR32(EntityRef self) {
    Value ultimatestress = express_getattr(self, "UltimateStress");
    if (!Value(!(exists(ultimatestress)) || (ultimatestress >= 0.)).truthy()) return false;
    return true;
}

bool IfcMechanicalSteelMaterialProperties_WR33(EntityRef self) {
    Value hardeningmodule = express_getattr(self, "HardeningModule");
    if (!Value(!(exists(hardeningmodule)) || (hardeningmodule >= 0.)).truthy()) return false;
    return true;
}

bool IfcMechanicalSteelMaterialProperties_WR34(EntityRef self) {
    Value proportionalstress = express_getattr(self, "ProportionalStress");
    if (!Value(!(exists(proportionalstress)) || (proportionalstress >= 0.)).truthy()) return false;
    return true;
}

bool IfcMove_WR1(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "OperatesOn"))) >= 1).truthy()) return false;
    return true;
}

bool IfcMove_WR2(EntityRef self) {
    Value operateson = express_getattr(self, "OperatesOn");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(operateson)) { if (Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp2 : ifcapi::express::iter(express_getattr(temp, "RelatedObjects"))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcactor")), typeof_(temp2))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcequipmentelement")), typeof_(temp2))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfurnishingelement")), typeof_(temp2)))).truthy()) __r.append(temp2); } return __r; })())) >= 1).truthy()) __r.append(temp); } return __r; })())) >= 1).truthy()) return false;
    return true;
}

bool IfcMove_WR3(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcNamedUnit_WR1(EntityRef self) {
    if (!Value(IfcCorrectDimensions(express_getattr(self, "UnitType"), express_getattr(self, "Dimensions"))).truthy()) return false;
    return true;
}

bool IfcObject_WR1(EntityRef self) {
    Value isdefinedby = express_getattr(self, "IsDefinedBy");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(isdefinedby)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcreldefinesbytype")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcObjective_WR21(EntityRef self) {
    Value objectivequalifier = express_getattr(self, "ObjectiveQualifier");
    if (!Value((objectivequalifier != Value(std::string("USERDEFINED"))) || ((objectivequalifier == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedQualifier"))))).truthy()) return false;
    return true;
}

bool IfcOccupant_WR31(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((!((predefinedtype == Value(std::string("USERDEFINED"))))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcOffsetCurve2D_WR1(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    if (!Value((express_getattr(basiscurve, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcOffsetCurve3D_WR1(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    if (!Value((express_getattr(basiscurve, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcOrientedEdge_WR1(EntityRef self) {
    Value edgeelement = express_getattr(self, "EdgeElement");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcorientededge")), typeof_(edgeelement))))).truthy()) return false;
    return true;
}

Value calc_IfcOrientedEdge_EdgeStart(EntityRef self) {
    Value edgeelement = express_getattr(self, "EdgeElement");
    Value orientation = express_getattr(self, "Orientation");
    return IfcBooleanChoose(orientation, express_getattr(edgeelement, "EdgeStart"), express_getattr(edgeelement, "EdgeEnd"));
}

Value calc_IfcOrientedEdge_EdgeEnd(EntityRef self) {
    Value edgeelement = express_getattr(self, "EdgeElement");
    Value orientation = express_getattr(self, "Orientation");
    return IfcBooleanChoose(orientation, express_getattr(edgeelement, "EdgeEnd"), express_getattr(edgeelement, "EdgeStart"));
}

bool IfcPath_WR1(EntityRef self) {
    if (!Value(IfcPathHeadToTail(self)).truthy()) return false;
    return true;
}

bool IfcPerson_WR1(EntityRef self) {
    Value familyname = express_getattr(self, "FamilyName");
    Value givenname = express_getattr(self, "GivenName");
    if (!Value(exists(familyname) || exists(givenname)).truthy()) return false;
    return true;
}

bool IfcPhysicalComplexQuantity_WR21(EntityRef self) {
    Value hasquantities = express_getattr(self, "HasQuantities");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(hasquantities)) { if (Value(self == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPile_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcPipeFittingType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcPipeSegmentType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcPixelTexture_WR21(EntityRef self) {
    Value width = express_getattr(self, "Width");
    if (!Value(width >= 1).truthy()) return false;
    return true;
}

bool IfcPixelTexture_WR22(EntityRef self) {
    Value height = express_getattr(self, "Height");
    if (!Value(height >= 1).truthy()) return false;
    return true;
}

bool IfcPixelTexture_WR23(EntityRef self) {
    Value colourcomponents = express_getattr(self, "ColourComponents");
    if (!Value(1 <= colourcomponents <= 4).truthy()) return false;
    return true;
}

bool IfcPixelTexture_WR24(EntityRef self) {
    Value width = express_getattr(self, "Width");
    Value height = express_getattr(self, "Height");
    Value pixel = express_getattr(self, "Pixel");
    if (!Value(sizeof_(pixel) == (width * height)).truthy()) return false;
    return true;
}

Value calc_IfcPlacement_Dim(EntityRef self) {
    Value location = express_getattr(self, "Location");
    return express_getattr(location, "Dim");
}

Value calc_IfcPointOnCurve_Dim(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    return express_getattr(basiscurve, "Dim");
}

Value calc_IfcPointOnSurface_Dim(EntityRef self) {
    Value basissurface = express_getattr(self, "BasisSurface");
    return express_getattr(basissurface, "Dim");
}

bool IfcPolyLoop_WR21(EntityRef self) {
    Value polygon = express_getattr(self, "Polygon");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(polygon)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(polygon, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPolygonalBoundedHalfSpace_WR41(EntityRef self) {
    Value polygonalboundary = express_getattr(self, "PolygonalBoundary");
    if (!Value((express_getattr(polygonalboundary, "Dim")) == 2).truthy()) return false;
    return true;
}

bool IfcPolygonalBoundedHalfSpace_WR42(EntityRef self) {
    Value polygonalboundary = express_getattr(self, "PolygonalBoundary");
    if (!Value((sizeof_(typeof_(polygonalboundary) * (Value::make_list({Value(std::string("ifc2x3.ifcpolyline")), Value(std::string("ifc2x3.ifccompositecurve"))})))) == 1).truthy()) return false;
    return true;
}

bool IfcPolyline_WR41(EntityRef self) {
    Value points = express_getattr(self, "Points");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(points)) { if (Value((express_getattr(temp, "Dim")) != (express_getattr(express_getitem(points, 1 - 1), "Dim"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPostalAddress_WR1(EntityRef self) {
    Value internallocation = express_getattr(self, "InternalLocation");
    Value addresslines = express_getattr(self, "AddressLines");
    Value postalbox = express_getattr(self, "PostalBox");
    Value town = express_getattr(self, "Town");
    Value region = express_getattr(self, "Region");
    Value postalcode = express_getattr(self, "PostalCode");
    Value country = express_getattr(self, "Country");
    if (!Value(exists(internallocation) || exists(addresslines) || exists(postalbox) || exists(postalcode) || exists(town) || exists(region) || exists(country)).truthy()) return false;
    return true;
}

bool IfcPreDefinedDimensionSymbol_WR31(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("arclength")), Value(std::string("conicaltaper")), Value(std::string("counterbore")), Value(std::string("countersink")), Value(std::string("depth")), Value(std::string("diameter")), Value(std::string("plusminus")), Value(std::string("radius")), Value(std::string("slope")), Value(std::string("sphericaldiameter")), Value(std::string("sphericalradius")), Value(std::string("square"))}))).truthy()) return false;
    return true;
}

bool IfcPreDefinedPointMarkerSymbol_WR31(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("asterisk")), Value(std::string("circle")), Value(std::string("dot")), Value(std::string("plus")), Value(std::string("square")), Value(std::string("triangle")), Value(std::string("x"))}))).truthy()) return false;
    return true;
}

bool IfcPreDefinedTerminatorSymbol_WR31(EntityRef self) {
    if (!Value(ifcapi::express::express_in(express_getattr(self, "Name"), Value::make_list({Value(std::string("blankedarrow")), Value(std::string("blankedbox")), Value(std::string("blankeddot")), Value(std::string("dimensionorigin")), Value(std::string("filledarrow")), Value(std::string("filledbox")), Value(std::string("filleddot")), Value(std::string("integralsymbol")), Value(std::string("openarrow")), Value(std::string("slash")), Value(std::string("unfilledarrow"))}))).truthy()) return false;
    return true;
}

bool IfcProcedure_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Decomposes"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcrelnests")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcProcedure_WR2(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "IsDecomposedBy"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcrelnests")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcProcedure_WR3(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcProcedure_WR4(EntityRef self) {
    Value proceduretype = express_getattr(self, "ProcedureType");
    if (!Value((proceduretype != Value(std::string("USERDEFINED"))) || ((proceduretype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedProcedureType"))))).truthy()) return false;
    return true;
}

bool IfcProduct_WR1(EntityRef self) {
    Value objectplacement = express_getattr(self, "ObjectPlacement");
    Value representation = express_getattr(self, "Representation");
    if (!Value((exists(representation) && exists(objectplacement)) || (exists(representation) && (!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcproductdefinitionshape")), typeof_(representation)))))) || !(exists(representation))).truthy()) return false;
    return true;
}

bool IfcProductDefinitionShape_WR11(EntityRef self) {
    Value representations = express_getattr(self, "Representations");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(representations)) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcshapemodel")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcProject_WR31(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcProject_WR32(EntityRef self) {
    Value representationcontexts = express_getattr(self, "RepresentationContexts");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(representationcontexts)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgeometricrepresentationsubcontext")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcProject_WR33(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "Decomposes"))) == 0).truthy()) return false;
    return true;
}

bool IfcPropertyBoundedValue_WR21(EntityRef self) {
    Value upperboundvalue = express_getattr(self, "UpperBoundValue");
    Value lowerboundvalue = express_getattr(self, "LowerBoundValue");
    if (!Value(!(exists(upperboundvalue)) || !(exists(lowerboundvalue)) || (typeof_(upperboundvalue) == typeof_(lowerboundvalue))).truthy()) return false;
    return true;
}

bool IfcPropertyBoundedValue_WR22(EntityRef self) {
    Value upperboundvalue = express_getattr(self, "UpperBoundValue");
    Value lowerboundvalue = express_getattr(self, "LowerBoundValue");
    if (!Value(exists(upperboundvalue) || exists(lowerboundvalue)).truthy()) return false;
    return true;
}

bool IfcPropertyDependencyRelationship_WR1(EntityRef self) {
    Value dependingproperty = express_getattr(self, "DependingProperty");
    Value dependantproperty = express_getattr(self, "DependantProperty");
    if (!Value(dependingproperty != dependantproperty).truthy()) return false;
    return true;
}

bool IfcPropertyEnumeratedValue_WR1(EntityRef self) {
    Value enumerationvalues = express_getattr(self, "EnumerationValues");
    Value enumerationreference = express_getattr(self, "EnumerationReference");
    if (!Value(!(exists(enumerationreference)) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(enumerationvalues)) { if (Value(ifcapi::express::express_in(temp, express_getattr(enumerationreference, "EnumerationValues"))).truthy()) __r.append(temp); } return __r; })())) == sizeof_(enumerationvalues))).truthy()) return false;
    return true;
}

bool IfcPropertyEnumeration_WR01(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "EnumerationValues"))) { if (Value(!(((typeof_(express_getitem(express_getattr(self, "EnumerationValues"), 1 - 1))) == typeof_(temp)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPropertyListValue_WR31(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "ListValues"))) { if (Value(!(((typeof_(express_getitem(express_getattr(self, "ListValues"), 1 - 1))) == typeof_(temp)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPropertySet_WR31(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcPropertySet_WR32(EntityRef self) {
    Value hasproperties = express_getattr(self, "HasProperties");
    if (!Value(IfcUniquePropertyName(hasproperties)).truthy()) return false;
    return true;
}

bool IfcPropertyTableValue_WR1(EntityRef self) {
    Value definingvalues = express_getattr(self, "DefiningValues");
    Value definedvalues = express_getattr(self, "DefinedValues");
    if (!Value(sizeof_(definingvalues) == sizeof_(definedvalues)).truthy()) return false;
    return true;
}

bool IfcPropertyTableValue_WR2(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "DefiningValues"))) { if (Value(typeof_(temp) != (typeof_(express_getitem(express_getattr(self, "DefiningValues"), 1 - 1)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcPropertyTableValue_WR3(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "DefinedValues"))) { if (Value(typeof_(temp) != (typeof_(express_getitem(express_getattr(self, "DefinedValues"), 1 - 1)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcProxy_WR1(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcPumpType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcQuantityArea_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("AREAUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityArea_WR22(EntityRef self) {
    Value areavalue = express_getattr(self, "AreaValue");
    if (!Value(areavalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityCount_WR21(EntityRef self) {
    Value countvalue = express_getattr(self, "CountValue");
    if (!Value(countvalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityLength_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("LENGTHUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityLength_WR22(EntityRef self) {
    Value lengthvalue = express_getattr(self, "LengthValue");
    if (!Value(lengthvalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityTime_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("TIMEUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityTime_WR22(EntityRef self) {
    Value timevalue = express_getattr(self, "TimeValue");
    if (!Value(timevalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityVolume_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("VOLUMEUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityVolume_WR22(EntityRef self) {
    Value volumevalue = express_getattr(self, "VolumeValue");
    if (!Value(volumevalue >= 0.).truthy()) return false;
    return true;
}

bool IfcQuantityWeight_WR21(EntityRef self) {
    if (!Value((!((exists(express_getattr(self, "Unit"))))) || ((express_getattr(express_getattr(self, "Unit"), "UnitType")) == Value(std::string("MASSUNIT")))).truthy()) return false;
    return true;
}

bool IfcQuantityWeight_WR22(EntityRef self) {
    Value weightvalue = express_getattr(self, "WeightValue");
    if (!Value(weightvalue >= 0.).truthy()) return false;
    return true;
}

bool IfcRailing_WR61(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcRamp_WR1(EntityRef self) {
    if (!Value(((hiindex(express_getattr(self, "IsDecomposedBy"))) == 0) || (((hiindex(express_getattr(self, "IsDecomposedBy"))) == 1) && (!((exists(express_getattr(self, "Representation"))))))).truthy()) return false;
    return true;
}

bool IfcRationalBezierCurve_WR1(EntityRef self) {
    Value weightsdata = express_getattr(self, "WeightsData");
    if (!Value(sizeof_(weightsdata) == (sizeof_(express_getattr(self, "ControlPointsList")))).truthy()) return false;
    return true;
}

bool IfcRationalBezierCurve_WR2(EntityRef self) {
    if (!Value(IfcCurveWeightsPositive(self)).truthy()) return false;
    return true;
}

Value calc_IfcRationalBezierCurve_Weights(EntityRef self) {
    Value weightsdata = express_getattr(self, "WeightsData");
    return IfcListToArray(weightsdata, 0, express_getattr(self, "UpperIndexOnControlPoints"));
}

bool IfcRectangleHollowProfileDef_WR31(EntityRef self) {
    Value wallthickness = express_getattr(self, "WallThickness");
    if (!Value((wallthickness < ((express_getattr(self, "XDim")) / 2.)) && (wallthickness < ((express_getattr(self, "YDim")) / 2.))).truthy()) return false;
    return true;
}

bool IfcRectangleHollowProfileDef_WR32(EntityRef self) {
    Value outerfilletradius = express_getattr(self, "OuterFilletRadius");
    if (!Value(!(exists(outerfilletradius)) || ((outerfilletradius <= ((express_getattr(self, "XDim")) / 2.)) && (outerfilletradius <= ((express_getattr(self, "YDim")) / 2.)))).truthy()) return false;
    return true;
}

bool IfcRectangleHollowProfileDef_WR33(EntityRef self) {
    Value wallthickness = express_getattr(self, "WallThickness");
    Value innerfilletradius = express_getattr(self, "InnerFilletRadius");
    if (!Value(!(exists(innerfilletradius)) || ((innerfilletradius <= (((express_getattr(self, "XDim")) / 2.) - wallthickness)) && (innerfilletradius <= (((express_getattr(self, "YDim")) / 2.) - wallthickness)))).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_WR1(EntityRef self) {
    Value u1 = express_getattr(self, "U1");
    Value u2 = express_getattr(self, "U2");
    if (!Value(u1 != u2).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_WR2(EntityRef self) {
    Value v1 = express_getattr(self, "V1");
    Value v2 = express_getattr(self, "V2");
    if (!Value(v1 != v2).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_WR3(EntityRef self) {
    Value basissurface = express_getattr(self, "BasisSurface");
    Value u1 = express_getattr(self, "U1");
    Value u2 = express_getattr(self, "U2");
    Value usense = express_getattr(self, "Usense");
    if (!Value(((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcelementarysurface")), typeof_(basissurface))) && (!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcplane")), typeof_(basissurface)))))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurfaceofrevolution")), typeof_(basissurface))) || (usense == (u2 > u1))).truthy()) return false;
    return true;
}

bool IfcRectangularTrimmedSurface_WR4(EntityRef self) {
    Value v1 = express_getattr(self, "V1");
    Value v2 = express_getattr(self, "V2");
    Value vsense = express_getattr(self, "Vsense");
    if (!Value(vsense == (v2 > v1)).truthy()) return false;
    return true;
}

Value calc_IfcRectangularTrimmedSurface_Dim(EntityRef self) {
    Value basissurface = express_getattr(self, "BasisSurface");
    return express_getattr(basissurface, "Dim");
}

bool IfcReinforcingBar_WR1(EntityRef self) {
    Value barrole = express_getattr(self, "BarRole");
    if (!Value((barrole != Value(std::string("USERDEFINED"))) || ((barrole == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcRelAssigns_WR1(EntityRef self) {
    Value relatedobjects = express_getattr(self, "RelatedObjects");
    Value relatedobjectstype = express_getattr(self, "RelatedObjectsType");
    if (!Value(IfcCorrectObjectAssignment(relatedobjectstype, relatedobjects)).truthy()) return false;
    return true;
}

bool IfcRelAssignsTasks_WR1(EntityRef self) {
    if (!Value((hiindex(express_getattr(self, "RelatedObjects"))) == 1).truthy()) return false;
    return true;
}

bool IfcRelAssignsTasks_WR2(EntityRef self) {
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifctask")), typeof_(express_getitem(express_getattr(self, "RelatedObjects"), 1 - 1)))).truthy()) return false;
    return true;
}

bool IfcRelAssignsTasks_WR3(EntityRef self) {
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcworkcontrol")), typeof_(express_getattr(self, "RelatingControl")))).truthy()) return false;
    return true;
}

bool IfcRelAssignsToActor_WR1(EntityRef self) {
    Value relatingactor = express_getattr(self, "RelatingActor");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingactor == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToControl_WR1(EntityRef self) {
    Value relatingcontrol = express_getattr(self, "RelatingControl");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingcontrol == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToGroup_WR1(EntityRef self) {
    Value relatinggroup = express_getattr(self, "RelatingGroup");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatinggroup == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToProcess_WR1(EntityRef self) {
    Value relatingprocess = express_getattr(self, "RelatingProcess");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingprocess == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToProduct_WR1(EntityRef self) {
    Value relatingproduct = express_getattr(self, "RelatingProduct");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingproduct == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssignsToResource_WR1(EntityRef self) {
    Value relatingresource = express_getattr(self, "RelatingResource");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(relatingresource == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssociates_WR21(EntityRef self) {
    Value relatedobjects = express_getattr(self, "RelatedObjects");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedobjects)) { if (Value(!(((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcobjectdefinition")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcpropertydefinition")), typeof_(temp)))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssociatesMaterial_WR21(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcfeatureelementsubtraction")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvirtualelement")), typeof_(temp)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelAssociatesMaterial_WR22(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value((!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcproduct")), typeof_(temp))))) && (!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifctypeproduct")), typeof_(temp)))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelConnectsElements_WR31(EntityRef self) {
    Value relatingelement = express_getattr(self, "RelatingElement");
    Value relatedelement = express_getattr(self, "RelatedElement");
    if (!Value(relatingelement != relatedelement).truthy()) return false;
    return true;
}

bool IfcRelContainedInSpatialStructure_WR31(EntityRef self) {
    Value relatedelements = express_getattr(self, "RelatedElements");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedelements)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcspatialstructureelement")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelDecomposes_WR31(EntityRef self) {
    Value relatingobject = express_getattr(self, "RelatingObject");
    Value relatedobjects = express_getattr(self, "RelatedObjects");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedobjects)) { if (Value(relatingobject == temp).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelNests_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(!(((typeof_(express_getattr(self, "RelatingObject"))) == typeof_(temp)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelOverridesProperties_WR1(EntityRef self) {
    if (!Value((sizeof_(express_getattr(self, "RelatedObjects"))) == 1).truthy()) return false;
    return true;
}

bool IfcRelReferencedInSpatialStructure_WR31(EntityRef self) {
    Value relatedelements = express_getattr(self, "RelatedElements");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(relatedelements)) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcspatialstructureelement")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelSchedulesCostItems_WR11(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "RelatedObjects"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifccostitem")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRelSchedulesCostItems_WR12(EntityRef self) {
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifccostschedule")), typeof_(express_getattr(self, "RelatingControl")))).truthy()) return false;
    return true;
}

bool IfcRelSequence_WR1(EntityRef self) {
    Value relatingprocess = express_getattr(self, "RelatingProcess");
    Value relatedprocess = express_getattr(self, "RelatedProcess");
    if (!Value(relatingprocess != relatedprocess).truthy()) return false;
    return true;
}

bool IfcRelSpaceBoundary_WR1(EntityRef self) {
    Value relatedbuildingelement = express_getattr(self, "RelatedBuildingElement");
    Value physicalorvirtualboundary = express_getattr(self, "PhysicalOrVirtualBoundary");
    if (!Value(((physicalorvirtualboundary == Value(std::string("Physical"))) && (exists(relatedbuildingelement) && (!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvirtualelement")), typeof_(relatedbuildingelement))))))) || ((physicalorvirtualboundary == Value(std::string("Virtual"))) && (!(exists(relatedbuildingelement)) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcvirtualelement")), typeof_(relatedbuildingelement))))) || (physicalorvirtualboundary == Value(std::string("NotDefined")))).truthy()) return false;
    return true;
}

bool IfcRevolvedAreaSolid_WR31(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value((express_getitem(express_getattr(express_getattr(axis, "Location"), "Coordinates"), 3 - 1)) == 0.0).truthy()) return false;
    return true;
}

bool IfcRevolvedAreaSolid_WR32(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    if (!Value((express_getitem(express_getattr(express_getattr(axis, "Z"), "DirectionRatios"), 3 - 1)) == 0.0).truthy()) return false;
    return true;
}

Value calc_IfcRevolvedAreaSolid_AxisLine(EntityRef self) {
    Value axis = express_getattr(self, "Axis");
    return IfcLine(express_getattr(axis, "Location"),  IfcVector(express_getattr(axis, "Z"),  1.0));
}

bool IfcRoof_WR1(EntityRef self) {
    if (!Value(((hiindex(express_getattr(self, "IsDecomposedBy"))) == 0) || (((hiindex(express_getattr(self, "IsDecomposedBy"))) == 1) && (!((exists(express_getattr(self, "Representation"))))))).truthy()) return false;
    return true;
}

bool IfcRoundedRectangleProfileDef_WR31(EntityRef self) {
    Value roundingradius = express_getattr(self, "RoundingRadius");
    if (!Value((roundingradius <= ((express_getattr(self, "XDim")) / 2.)) && (roundingradius <= ((express_getattr(self, "YDim")) / 2.))).truthy()) return false;
    return true;
}

Value calc_IfcSIUnit_Dimensions(EntityRef self) {
    return IfcDimensionsForSiUnit(express_getattr(self, "Name"));
}

bool IfcSectionedSpine_WR1(EntityRef self) {
    Value crosssections = express_getattr(self, "CrossSections");
    Value crosssectionpositions = express_getattr(self, "CrossSectionPositions");
    if (!Value(sizeof_(crosssections) == sizeof_(crosssectionpositions)).truthy()) return false;
    return true;
}

bool IfcSectionedSpine_WR2(EntityRef self) {
    Value crosssections = express_getattr(self, "CrossSections");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(crosssections)) { if (Value((express_getattr(express_getitem(crosssections, 1 - 1), "ProfileType")) != (express_getattr(temp, "ProfileType"))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcSectionedSpine_WR3(EntityRef self) {
    Value spinecurve = express_getattr(self, "SpineCurve");
    if (!Value((express_getattr(spinecurve, "Dim")) == 3).truthy()) return false;
    return true;
}

Value calc_IfcSectionedSpine_Dim(EntityRef self) {
    return 3;
}

bool IfcServiceLifeFactor_WR31(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((!((predefinedtype == Value(std::string("USERDEFINED"))))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcShapeModel_WR11(EntityRef self) {
    Value ofshapeaspect = express_getattr(self, "OfShapeAspect");
    if (!Value(((sizeof_(express_getattr(self, "OfProductRepresentation"))) == 1) ^ ((sizeof_(express_getattr(self, "RepresentationMap"))) == 1) ^ (sizeof_(ofshapeaspect) == 1)).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_WR21(EntityRef self) {
    if (!Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcgeometricrepresentationcontext")), typeof_(express_getattr(self, "ContextOfItems")))).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_WR22(EntityRef self) {
    Value items = express_getattr(self, "Items");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(items)) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifctopologicalrepresentationitem")), typeof_(temp))) && (!(((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcvertexpoint")), Value(std::string("ifc2x3.ifcedgecurve")), Value(std::string("ifc2x3.ifcfacesurface"))})) * typeof_(temp))) == 1)))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_WR23(EntityRef self) {
    if (!Value(exists(express_getattr(self, "RepresentationType"))).truthy()) return false;
    return true;
}

bool IfcShapeRepresentation_WR24(EntityRef self) {
    if (!Value(IfcShapeRepresentationTypes(express_getattr(self, "RepresentationType"), express_getattr(self, "Items"))).truthy()) return false;
    return true;
}

Value calc_IfcShellBasedSurfaceModel_Dim(EntityRef self) {
    return 3;
}

bool IfcSlab_WR61(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value(!(exists(predefinedtype)) || (predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

Value calc_IfcSolidModel_Dim(EntityRef self) {
    return 3;
}

bool IfcSpaceHeaterType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcSpatialStructureElement_WR41(EntityRef self) {
    if (!Value(((hiindex(express_getattr(self, "Decomposes"))) == 1) && (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcrelaggregates")), typeof_(express_getitem(express_getattr(self, "Decomposes"), 1 - 1)))) && ((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcproject")), typeof_(express_getattr(express_getitem(express_getattr(self, "Decomposes"), 1 - 1), "RelatingObject")))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcspatialstructureelement")), typeof_(express_getattr(express_getitem(express_getattr(self, "Decomposes"), 1 - 1), "RelatingObject")))))).truthy()) return false;
    return true;
}

bool IfcStair_WR1(EntityRef self) {
    if (!Value(((hiindex(express_getattr(self, "IsDecomposedBy"))) == 0) || (((hiindex(express_getattr(self, "IsDecomposedBy"))) == 1) && (!((exists(express_getattr(self, "Representation"))))))).truthy()) return false;
    return true;
}

bool IfcStructuralLinearAction_WR61(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcstructuralloadlinearforce")), Value(std::string("ifc2x3.ifcstructuralloadtemperature"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

Value calc_IfcStructuralLinearActionVarying_VaryingAppliedLoads(EntityRef self) {
    Value subsequentappliedloads = express_getattr(self, "SubsequentAppliedLoads");
    return IfcAddToBeginOfList(express_getattr(self, "AppliedLoad"), subsequentappliedloads);
}

bool IfcStructuralPlanarAction_WR61(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcstructuralloadplanarforce")), Value(std::string("ifc2x3.ifcstructuralloadtemperature"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

Value calc_IfcStructuralPlanarActionVarying_VaryingAppliedLoads(EntityRef self) {
    Value subsequentappliedloads = express_getattr(self, "SubsequentAppliedLoads");
    return IfcAddToBeginOfList(express_getattr(self, "AppliedLoad"), subsequentappliedloads);
}

bool IfcStructuralPointAction_WR61(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcstructuralloadsingleforce")), Value(std::string("ifc2x3.ifcstructuralloadsingledisplacement"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

bool IfcStructuralPointReaction_WR61(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcstructuralloadsingleforce")), Value(std::string("ifc2x3.ifcstructuralloadsingledisplacement"))})) * (typeof_(express_getattr(self, "AppliedLoad"))))) == 1).truthy()) return false;
    return true;
}

bool IfcStructuralProfileProperties_WR21(EntityRef self) {
    Value sheardeformationareay = express_getattr(self, "ShearDeformationAreaY");
    if (!Value(!(exists(sheardeformationareay)) || (sheardeformationareay >= 0.)).truthy()) return false;
    return true;
}

bool IfcStructuralProfileProperties_WR22(EntityRef self) {
    Value sheardeformationareaz = express_getattr(self, "ShearDeformationAreaZ");
    if (!Value(!(exists(sheardeformationareaz)) || (sheardeformationareaz >= 0.)).truthy()) return false;
    return true;
}

bool IfcStructuralSteelProfileProperties_WR31(EntityRef self) {
    Value shearareay = express_getattr(self, "ShearAreaY");
    if (!Value(!(exists(shearareay)) || (shearareay >= 0.)).truthy()) return false;
    return true;
}

bool IfcStructuralSteelProfileProperties_WR32(EntityRef self) {
    Value shearareaz = express_getattr(self, "ShearAreaZ");
    if (!Value(!(exists(shearareaz)) || (shearareaz >= 0.)).truthy()) return false;
    return true;
}

bool IfcStructuralSurfaceMemberVarying_WR61(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Thickness"))).truthy()) return false;
    return true;
}

bool IfcStructuralSurfaceMemberVarying_WR62(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getattr(self, "VaryingThicknessLocation"), "ShapeRepresentations"))) { if (Value(!(((sizeof_(express_getattr(temp, "Items"))) == 1))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcStructuralSurfaceMemberVarying_WR63(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getattr(self, "VaryingThicknessLocation"), "ShapeRepresentations"))) { if (Value(!(((ifcapi::express::express_in(Value(std::string("ifc2x3.ifccartesianpoint")), typeof_(express_getitem(express_getattr(temp, "Items"), 1 - 1)))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcpointonsurface")), typeof_(express_getitem(express_getattr(temp, "Items"), 1 - 1))))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

Value calc_IfcStructuralSurfaceMemberVarying_VaryingThickness(EntityRef self) {
    Value subsequentthickness = express_getattr(self, "SubsequentThickness");
    return IfcAddToBeginOfList(express_getattr(self, "Thickness"), subsequentthickness);
}

bool IfcStructuredDimensionCallout_WR31(EntityRef self) {
    Value contents = express_getattr(self, "Contents");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& ato : ifcapi::express::iter(([&]() { auto __r = Value::make_list({}); for (auto& con : ifcapi::express::iter(express_getattr(self, "contents"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcannotationtextoccurrence")), typeof_(con))).truthy()) __r.append(con); } return __r; })())) { if (Value(!((ifcapi::express::express_in(express_getattr(ato, "Name"), Value::make_list({Value(std::string("dimensionvalue")), Value(std::string("tolerancevalue")), Value(std::string("unittext")), Value(std::string("prefixtext")), Value(std::string("suffixtext"))}))))).truthy()) __r.append(ato); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcStyledItem_WR11(EntityRef self) {
    Value styles = express_getattr(self, "Styles");
    if (!Value(sizeof_(styles) == 1).truthy()) return false;
    return true;
}

bool IfcStyledItem_WR12(EntityRef self) {
    Value item = express_getattr(self, "Item");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcstyleditem")), typeof_(item))))).truthy()) return false;
    return true;
}

bool IfcStyledRepresentation_WR21(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Items"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcstyleditem")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcSurfaceOfLinearExtrusion_WR41(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    if (!Value(depth > 0.).truthy()) return false;
    return true;
}

Value calc_IfcSurfaceOfLinearExtrusion_ExtrusionAxis(EntityRef self) {
    Value extrudeddirection = express_getattr(self, "ExtrudedDirection");
    Value depth = express_getattr(self, "Depth");
    return IfcVector(extrudeddirection,  depth);
}

Value calc_IfcSurfaceOfRevolution_AxisLine(EntityRef self) {
    Value axisposition = express_getattr(self, "AxisPosition");
    return IfcLine(express_getattr(axisposition, "Location"),  IfcVector(express_getattr(axisposition, "Z"),  1.0));
}

bool IfcSurfaceStyle_WR11(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurfacestyleshading")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_WR12(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurfacestylelighting")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_WR13(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurfacestylerefraction")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_WR14(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcsurfacestylewithtextures")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSurfaceStyle_WR15(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& style : ifcapi::express::iter(express_getattr(self, "Styles"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcexternallydefinedsurfacestyle")), typeof_(style))).truthy()) __r.append(style); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcSweptAreaSolid_WR22(EntityRef self) {
    Value sweptarea = express_getattr(self, "SweptArea");
    if (!Value((express_getattr(sweptarea, "ProfileType")) == Value(std::string("Area"))).truthy()) return false;
    return true;
}

bool IfcSweptDiskSolid_WR1(EntityRef self) {
    Value directrix = express_getattr(self, "Directrix");
    if (!Value((express_getattr(directrix, "Dim")) == 3).truthy()) return false;
    return true;
}

bool IfcSweptDiskSolid_WR2(EntityRef self) {
    Value radius = express_getattr(self, "Radius");
    Value innerradius = express_getattr(self, "InnerRadius");
    if (!Value(!(exists(innerradius)) || (radius > innerradius)).truthy()) return false;
    return true;
}

bool IfcSweptSurface_WR1(EntityRef self) {
    Value sweptcurve = express_getattr(self, "SweptCurve");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcderivedprofiledef")), typeof_(sweptcurve))))).truthy()) return false;
    return true;
}

bool IfcSweptSurface_WR2(EntityRef self) {
    Value sweptcurve = express_getattr(self, "SweptCurve");
    if (!Value((express_getattr(sweptcurve, "ProfileType")) == Value(std::string("Curve"))).truthy()) return false;
    return true;
}

Value calc_IfcSweptSurface_Dim(EntityRef self) {
    Value position = express_getattr(self, "Position");
    return express_getattr(position, "Dim");
}

bool IfcTShapeProfileDef_WR1(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value(flangethickness < depth).truthy()) return false;
    return true;
}

bool IfcTShapeProfileDef_WR2(EntityRef self) {
    Value flangewidth = express_getattr(self, "FlangeWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    if (!Value(webthickness < flangewidth).truthy()) return false;
    return true;
}

bool IfcTable_WR1(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(rows)) { if (Value((hiindex(express_getattr(temp, "RowCells"))) != (hiindex(express_getattr(express_getitem(rows, 1 - 1), "RowCells")))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcTable_WR2(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(rows)) { if (Value((hiindex(express_getattr(temp, "RowCells"))) != (hiindex(express_getattr(express_getitem(rows, 1 - 1), "RowCells")))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcTable_WR3(EntityRef self) {
    Value numberofheadings = express_getattr(self, "NumberOfHeadings");
    if (!Value(0 <= numberofheadings <= 1).truthy()) return false;
    return true;
}

Value calc_IfcTable_NumberOfCellsInRow(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    return hiindex(express_getattr(express_getitem(rows, 1 - 1), "RowCells"));
}

Value calc_IfcTable_NumberOfHeadings(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    return sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(rows)) { if (Value(express_getattr(temp, "IsHeading")).truthy()) __r.append(temp); } return __r; })());
}

Value calc_IfcTable_NumberOfDataRows(EntityRef self) {
    Value rows = express_getattr(self, "Rows");
    return sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(rows)) { if (Value(!((express_getattr(temp, "IsHeading")))).truthy()) __r.append(temp); } return __r; })());
}

bool IfcTankType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcTask_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Decomposes"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcrelnests")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcTask_WR2(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "IsDecomposedBy"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcrelnests")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcTask_WR3(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcTelecomAddress_WR1(EntityRef self) {
    Value telephonenumbers = express_getattr(self, "TelephoneNumbers");
    Value facsimilenumbers = express_getattr(self, "FacsimileNumbers");
    Value pagernumber = express_getattr(self, "PagerNumber");
    Value electronicmailaddresses = express_getattr(self, "ElectronicMailAddresses");
    Value wwwhomepageurl = express_getattr(self, "WWWHomePageURL");
    if (!Value(exists(telephonenumbers) || exists(pagernumber) || exists(facsimilenumbers) || exists(electronicmailaddresses) || exists(wwwhomepageurl)).truthy()) return false;
    return true;
}

bool IfcTendon_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ObjectType"))))).truthy()) return false;
    return true;
}

bool IfcTextLiteralWithExtent_WR31(EntityRef self) {
    Value extent = express_getattr(self, "Extent");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcplanarbox")), typeof_(extent))))).truthy()) return false;
    return true;
}

bool IfcTextStyleFontModel_WR31(EntityRef self) {
    if (!Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifclengthmeasure")), typeof_(express_getattr(self, "FontSize")))) && ((express_getattr(self, "FontSize")) > 0.)).truthy()) return false;
    return true;
}

bool IfcTextureMap_WR11(EntityRef self) {
    if (!Value((sizeof_((Value::make_list({Value(std::string("ifc2x3.ifcshellbasedsurfacemodel")), Value(std::string("ifc2x3.ifcfacebasedsurfacemodel")), Value(std::string("ifc2x3.ifcfacetedbrep")), Value(std::string("ifc2x3.ifcfacetedbrepwithvoids"))})) * (typeof_(express_getattr(express_getitem(express_getattr(self, "AnnotatedSurface"), 1 - 1), "Item"))))) >= 1).truthy()) return false;
    return true;
}

bool IfcTimeSeriesSchedule_WR41(EntityRef self) {
    Value timeseriesscheduletype = express_getattr(self, "TimeSeriesScheduleType");
    if (!Value((!((timeseriesscheduletype == Value(std::string("USERDEFINED"))))) || (exists(express_getattr(self, "ObjectType")))).truthy()) return false;
    return true;
}

bool IfcTopologyRepresentation_WR21(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "Items"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifctopologicalrepresentationitem")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcTopologyRepresentation_WR22(EntityRef self) {
    if (!Value(exists(express_getattr(self, "RepresentationType"))).truthy()) return false;
    return true;
}

bool IfcTopologyRepresentation_WR23(EntityRef self) {
    if (!Value(IfcTopologyRepresentationTypes(express_getattr(self, "RepresentationType"), express_getattr(self, "Items"))).truthy()) return false;
    return true;
}

bool IfcTrimmedCurve_WR41(EntityRef self) {
    Value trim1 = express_getattr(self, "Trim1");
    if (!Value((hiindex(trim1) == 1) || ((typeof_(express_getitem(trim1, 1 - 1))) != (typeof_(express_getitem(trim1, 2 - 1))))).truthy()) return false;
    return true;
}

bool IfcTrimmedCurve_WR42(EntityRef self) {
    Value trim2 = express_getattr(self, "Trim2");
    if (!Value((hiindex(trim2) == 1) || ((typeof_(express_getitem(trim2, 1 - 1))) != (typeof_(express_getitem(trim2, 2 - 1))))).truthy()) return false;
    return true;
}

bool IfcTrimmedCurve_WR43(EntityRef self) {
    Value basiscurve = express_getattr(self, "BasisCurve");
    if (!Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcboundedcurve")), typeof_(basiscurve))))).truthy()) return false;
    return true;
}

bool IfcTubeBundleType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcTypeObject_WR1(EntityRef self) {
    if (!Value(exists(express_getattr(self, "Name"))).truthy()) return false;
    return true;
}

bool IfcTypeProduct_WR41(EntityRef self) {
    if (!Value((!((exists(express_getitem(express_getattr(self, "ObjectTypeOf"), 1 - 1))))) || ((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getitem(express_getattr(self, "ObjectTypeOf"), 1 - 1), "RelatedObjects"))) { if (Value(!((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcproduct")), typeof_(temp))))).truthy()) __r.append(temp); } return __r; })())) == 0)).truthy()) return false;
    return true;
}

bool IfcUShapeProfileDef_WR21(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value(flangethickness < (depth / 2.)).truthy()) return false;
    return true;
}

bool IfcUShapeProfileDef_WR22(EntityRef self) {
    Value flangewidth = express_getattr(self, "FlangeWidth");
    Value webthickness = express_getattr(self, "WebThickness");
    if (!Value(webthickness < flangewidth).truthy()) return false;
    return true;
}

bool IfcUnitAssignment_WR01(EntityRef self) {
    Value units = express_getattr(self, "Units");
    if (!Value(IfcCorrectUnitAssignment(units)).truthy()) return false;
    return true;
}

bool IfcUnitaryEquipmentType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcValveType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcVector_WR1(EntityRef self) {
    Value magnitude = express_getattr(self, "Magnitude");
    if (!Value(magnitude >= 0.0).truthy()) return false;
    return true;
}

Value calc_IfcVector_Dim(EntityRef self) {
    Value orientation = express_getattr(self, "Orientation");
    return express_getattr(orientation, "Dim");
}

bool IfcVibrationIsolatorType_WR1(EntityRef self) {
    Value predefinedtype = express_getattr(self, "PredefinedType");
    if (!Value((predefinedtype != Value(std::string("USERDEFINED"))) || ((predefinedtype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "ElementType"))))).truthy()) return false;
    return true;
}

bool IfcWall_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(self, "HasAssociations"))) { if (Value(ifcapi::express::express_in(Value(std::string("ifc2x3.ifcrelassociatesmaterial")), typeof_(temp))).truthy()) __r.append(temp); } return __r; })())) <= 1).truthy()) return false;
    return true;
}

bool IfcWallStandardCase_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(usedin(self, Value(std::string("ifc2x3.ifcrelassociates.relatedobjects"))))) { if (Value((ifcapi::express::express_in(Value(std::string("ifc2x3.ifcrelassociatesmaterial")), typeof_(temp))) && (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcmateriallayersetusage")), typeof_(express_getattr(temp, "RelatingMaterial"))))).truthy()) __r.append(temp); } return __r; })())) == 1).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR31(EntityRef self) {
    Value liningdepth = express_getattr(self, "LiningDepth");
    Value liningthickness = express_getattr(self, "LiningThickness");
    if (!Value(!((!(exists(liningdepth)) && exists(liningthickness)))).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR32(EntityRef self) {
    Value firsttransomoffset = express_getattr(self, "FirstTransomOffset");
    Value secondtransomoffset = express_getattr(self, "SecondTransomOffset");
    if (!Value(!((!(exists(firsttransomoffset)) && exists(secondtransomoffset)))).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR33(EntityRef self) {
    Value firstmullionoffset = express_getattr(self, "FirstMullionOffset");
    Value secondmullionoffset = express_getattr(self, "SecondMullionOffset");
    if (!Value(!((!(exists(firstmullionoffset)) && exists(secondmullionoffset)))).truthy()) return false;
    return true;
}

bool IfcWindowLiningProperties_WR34(EntityRef self) {
    if (!Value((exists(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))) && (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcwindowstyle")), typeof_(express_getitem(express_getattr(self, "DefinesType"), 1 - 1))))).truthy()) return false;
    return true;
}

bool IfcWorkControl_WR1(EntityRef self) {
    Value workcontroltype = express_getattr(self, "WorkControlType");
    if (!Value((workcontroltype != Value(std::string("USERDEFINED"))) || ((workcontroltype == Value(std::string("USERDEFINED"))) && (exists(express_getattr(self, "UserDefinedControlType"))))).truthy()) return false;
    return true;
}

bool IfcZShapeProfileDef_WR21(EntityRef self) {
    Value depth = express_getattr(self, "Depth");
    Value flangethickness = express_getattr(self, "FlangeThickness");
    if (!Value(flangethickness < (depth / 2.)).truthy()) return false;
    return true;
}

bool IfcZone_WR1(EntityRef self) {
    if (!Value((sizeof_(([&]() { auto __r = Value::make_list({}); for (auto& temp : ifcapi::express::iter(express_getattr(express_getattr(self, "IsGroupedBy"), "RelatedObjects"))) { if (Value(!(((ifcapi::express::express_in(Value(std::string("ifc2x3.ifczone")), typeof_(temp))) || (ifcapi::express::express_in(Value(std::string("ifc2x3.ifcspace")), typeof_(temp)))))).truthy()) __r.append(temp); } return __r; })())) == 0).truthy()) return false;
    return true;
}

bool IfcRepresentationContextSameWCS(IfcFile* file) {
    Value IfcGeometricRepresentationContext = ifcapi::express::file_by_type(file, "IfcGeometricRepresentationContext");
    Value isdifferent = false;
    if (Value(sizeof_(IfcGeometricRepresentationContext) > 1).truthy()) {
        for (Value i = 2; (Value(i) <= Value(hiindex(IfcGeometricRepresentationContext))).truthy(); i = i + Value((int64_t)1)) {
            if (Value((express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "WorldCoordinateSystem")) != (express_getattr(express_getitem(IfcGeometricRepresentationContext, i - 1), "WorldCoordinateSystem"))).truthy()) {
                isdifferent = (!((IfcSameValidPrecision(express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "Precision"), express_getattr(express_getitem(IfcGeometricRepresentationContext, i - 1), "Precision"))))) || (!((IfcSameAxis2Placement(express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "WorldCoordinateSystem"), express_getattr(express_getitem(IfcGeometricRepresentationContext, i - 1), "WorldCoordinateSystem"), express_getattr(express_getitem(IfcGeometricRepresentationContext, 1 - 1), "Precision")))));
                if (Value(isdifferent == true).truthy()) {
                    break;
                }
            }
        }
    }
    if (!Value(isdifferent == false).truthy()) return false;
    return true;
}

bool IfcSingleProjectInstance(IfcFile* file) {
    Value IfcProject = ifcapi::express::file_by_type(file, "IfcProject");
    if (!Value(sizeof_(IfcProject) <= 1).truthy()) return false;
    return true;
}


// --- derived-attribute registry ---
namespace { struct __derived_register_t { __derived_register_t() {
    register_derived("IFC2X3", "IfcAxis1Placement", "Z", (DeriveFn)&calc_IfcAxis1Placement_Z);
    register_derived("IFC2X3", "IfcAxis2Placement2D", "P", (DeriveFn)&calc_IfcAxis2Placement2D_P);
    register_derived("IFC2X3", "IfcAxis2Placement3D", "P", (DeriveFn)&calc_IfcAxis2Placement3D_P);
    register_derived("IFC2X3", "IfcBSplineCurve", "ControlPoints", (DeriveFn)&calc_IfcBSplineCurve_ControlPoints);
    register_derived("IFC2X3", "IfcBSplineCurve", "UpperIndexOnControlPoints", (DeriveFn)&calc_IfcBSplineCurve_UpperIndexOnControlPoints);
    register_derived("IFC2X3", "IfcBooleanResult", "Dim", (DeriveFn)&calc_IfcBooleanResult_Dim);
    register_derived("IFC2X3", "IfcBoundingBox", "Dim", (DeriveFn)&calc_IfcBoundingBox_Dim);
    register_derived("IFC2X3", "IfcCartesianPoint", "Dim", (DeriveFn)&calc_IfcCartesianPoint_Dim);
    register_derived("IFC2X3", "IfcCartesianTransformationOperator", "Scl", (DeriveFn)&calc_IfcCartesianTransformationOperator_Scl);
    register_derived("IFC2X3", "IfcCartesianTransformationOperator", "Dim", (DeriveFn)&calc_IfcCartesianTransformationOperator_Dim);
    register_derived("IFC2X3", "IfcCartesianTransformationOperator2D", "U", (DeriveFn)&calc_IfcCartesianTransformationOperator2D_U);
    register_derived("IFC2X3", "IfcCartesianTransformationOperator2DnonUniform", "Scl2", (DeriveFn)&calc_IfcCartesianTransformationOperator2DnonUniform_Scl2);
    register_derived("IFC2X3", "IfcCartesianTransformationOperator3D", "U", (DeriveFn)&calc_IfcCartesianTransformationOperator3D_U);
    register_derived("IFC2X3", "IfcCartesianTransformationOperator3DnonUniform", "Scl2", (DeriveFn)&calc_IfcCartesianTransformationOperator3DnonUniform_Scl2);
    register_derived("IFC2X3", "IfcCartesianTransformationOperator3DnonUniform", "Scl3", (DeriveFn)&calc_IfcCartesianTransformationOperator3DnonUniform_Scl3);
    register_derived("IFC2X3", "IfcCompositeCurve", "NSegments", (DeriveFn)&calc_IfcCompositeCurve_NSegments);
    register_derived("IFC2X3", "IfcCompositeCurve", "ClosedCurve", (DeriveFn)&calc_IfcCompositeCurve_ClosedCurve);
    register_derived("IFC2X3", "IfcCompositeCurveSegment", "Dim", (DeriveFn)&calc_IfcCompositeCurveSegment_Dim);
    register_derived("IFC2X3", "IfcCsgPrimitive3D", "Dim", (DeriveFn)&calc_IfcCsgPrimitive3D_Dim);
    register_derived("IFC2X3", "IfcCurve", "Dim", (DeriveFn)&calc_IfcCurve_Dim);
    register_derived("IFC2X3", "IfcCurveBoundedPlane", "Dim", (DeriveFn)&calc_IfcCurveBoundedPlane_Dim);
    register_derived("IFC2X3", "IfcDerivedUnit", "Dimensions", (DeriveFn)&calc_IfcDerivedUnit_Dimensions);
    register_derived("IFC2X3", "IfcDirection", "Dim", (DeriveFn)&calc_IfcDirection_Dim);
    register_derived("IFC2X3", "IfcEdgeLoop", "Ne", (DeriveFn)&calc_IfcEdgeLoop_Ne);
    register_derived("IFC2X3", "IfcElementarySurface", "Dim", (DeriveFn)&calc_IfcElementarySurface_Dim);
    register_derived("IFC2X3", "IfcFaceBasedSurfaceModel", "Dim", (DeriveFn)&calc_IfcFaceBasedSurfaceModel_Dim);
    register_derived("IFC2X3", "IfcGeometricRepresentationSubContext", "WorldCoordinateSystem", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_WorldCoordinateSystem);
    register_derived("IFC2X3", "IfcGeometricRepresentationSubContext", "CoordinateSpaceDimension", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_CoordinateSpaceDimension);
    register_derived("IFC2X3", "IfcGeometricRepresentationSubContext", "TrueNorth", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_TrueNorth);
    register_derived("IFC2X3", "IfcGeometricRepresentationSubContext", "Precision", (DeriveFn)&calc_IfcGeometricRepresentationSubContext_Precision);
    register_derived("IFC2X3", "IfcGeometricSet", "Dim", (DeriveFn)&calc_IfcGeometricSet_Dim);
    register_derived("IFC2X3", "IfcHalfSpaceSolid", "Dim", (DeriveFn)&calc_IfcHalfSpaceSolid_Dim);
    register_derived("IFC2X3", "IfcMaterialLayerSet", "TotalThickness", (DeriveFn)&calc_IfcMaterialLayerSet_TotalThickness);
    register_derived("IFC2X3", "IfcOrientedEdge", "EdgeStart", (DeriveFn)&calc_IfcOrientedEdge_EdgeStart);
    register_derived("IFC2X3", "IfcOrientedEdge", "EdgeEnd", (DeriveFn)&calc_IfcOrientedEdge_EdgeEnd);
    register_derived("IFC2X3", "IfcPlacement", "Dim", (DeriveFn)&calc_IfcPlacement_Dim);
    register_derived("IFC2X3", "IfcPointOnCurve", "Dim", (DeriveFn)&calc_IfcPointOnCurve_Dim);
    register_derived("IFC2X3", "IfcPointOnSurface", "Dim", (DeriveFn)&calc_IfcPointOnSurface_Dim);
    register_derived("IFC2X3", "IfcRationalBezierCurve", "Weights", (DeriveFn)&calc_IfcRationalBezierCurve_Weights);
    register_derived("IFC2X3", "IfcRectangularTrimmedSurface", "Dim", (DeriveFn)&calc_IfcRectangularTrimmedSurface_Dim);
    register_derived("IFC2X3", "IfcRevolvedAreaSolid", "AxisLine", (DeriveFn)&calc_IfcRevolvedAreaSolid_AxisLine);
    register_derived("IFC2X3", "IfcSIUnit", "Dimensions", (DeriveFn)&calc_IfcSIUnit_Dimensions);
    register_derived("IFC2X3", "IfcSectionedSpine", "Dim", (DeriveFn)&calc_IfcSectionedSpine_Dim);
    register_derived("IFC2X3", "IfcShellBasedSurfaceModel", "Dim", (DeriveFn)&calc_IfcShellBasedSurfaceModel_Dim);
    register_derived("IFC2X3", "IfcSolidModel", "Dim", (DeriveFn)&calc_IfcSolidModel_Dim);
    register_derived("IFC2X3", "IfcStructuralLinearActionVarying", "VaryingAppliedLoads", (DeriveFn)&calc_IfcStructuralLinearActionVarying_VaryingAppliedLoads);
    register_derived("IFC2X3", "IfcStructuralPlanarActionVarying", "VaryingAppliedLoads", (DeriveFn)&calc_IfcStructuralPlanarActionVarying_VaryingAppliedLoads);
    register_derived("IFC2X3", "IfcStructuralSurfaceMemberVarying", "VaryingThickness", (DeriveFn)&calc_IfcStructuralSurfaceMemberVarying_VaryingThickness);
    register_derived("IFC2X3", "IfcSurfaceOfLinearExtrusion", "ExtrusionAxis", (DeriveFn)&calc_IfcSurfaceOfLinearExtrusion_ExtrusionAxis);
    register_derived("IFC2X3", "IfcSurfaceOfRevolution", "AxisLine", (DeriveFn)&calc_IfcSurfaceOfRevolution_AxisLine);
    register_derived("IFC2X3", "IfcSweptSurface", "Dim", (DeriveFn)&calc_IfcSweptSurface_Dim);
    register_derived("IFC2X3", "IfcTable", "NumberOfCellsInRow", (DeriveFn)&calc_IfcTable_NumberOfCellsInRow);
    register_derived("IFC2X3", "IfcTable", "NumberOfHeadings", (DeriveFn)&calc_IfcTable_NumberOfHeadings);
    register_derived("IFC2X3", "IfcTable", "NumberOfDataRows", (DeriveFn)&calc_IfcTable_NumberOfDataRows);
    register_derived("IFC2X3", "IfcVector", "Dim", (DeriveFn)&calc_IfcVector_Dim);
} }; static __derived_register_t __derived_register_instance;}

}}}  // namespace ifcapi::express::IFC2X3
