#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/FLTDECLKLT_POT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/FLTDECLKLT_POT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../FLTDECKLT_EPWM.X/can.c ../FLTDECKLT_EPWM.X/in-out.c ../FLTDECKLT_EPWM.X/main.c ../FLTDECKLT_EPWM.X/mcu.c ../FLTDECKLT_EPWM.X/utils.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1160731302/can.o ${OBJECTDIR}/_ext/1160731302/in-out.o ${OBJECTDIR}/_ext/1160731302/main.o ${OBJECTDIR}/_ext/1160731302/mcu.o ${OBJECTDIR}/_ext/1160731302/utils.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1160731302/can.o.d ${OBJECTDIR}/_ext/1160731302/in-out.o.d ${OBJECTDIR}/_ext/1160731302/main.o.d ${OBJECTDIR}/_ext/1160731302/mcu.o.d ${OBJECTDIR}/_ext/1160731302/utils.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1160731302/can.o ${OBJECTDIR}/_ext/1160731302/in-out.o ${OBJECTDIR}/_ext/1160731302/main.o ${OBJECTDIR}/_ext/1160731302/mcu.o ${OBJECTDIR}/_ext/1160731302/utils.o

# Source Files
SOURCEFILES=../FLTDECKLT_EPWM.X/can.c ../FLTDECKLT_EPWM.X/in-out.c ../FLTDECKLT_EPWM.X/main.c ../FLTDECKLT_EPWM.X/mcu.c ../FLTDECKLT_EPWM.X/utils.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/FLTDECLKLT_POT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F46K80
MP_PROCESSOR_OPTION_LD=18f46k80
MP_LINKER_DEBUG_OPTION=  -u_DEBUGSTACK
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1160731302/can.o: ../FLTDECKLT_EPWM.X/can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/can.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/can.o   ../FLTDECKLT_EPWM.X/can.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/can.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/in-out.o: ../FLTDECKLT_EPWM.X/in-out.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/in-out.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/in-out.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/in-out.o   ../FLTDECKLT_EPWM.X/in-out.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/in-out.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/in-out.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/main.o: ../FLTDECKLT_EPWM.X/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/main.o   ../FLTDECKLT_EPWM.X/main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/mcu.o: ../FLTDECKLT_EPWM.X/mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/mcu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/mcu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/mcu.o   ../FLTDECKLT_EPWM.X/mcu.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/mcu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/mcu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/utils.o: ../FLTDECKLT_EPWM.X/utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/utils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/utils.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/utils.o   ../FLTDECKLT_EPWM.X/utils.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/utils.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/_ext/1160731302/can.o: ../FLTDECKLT_EPWM.X/can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/can.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/can.o   ../FLTDECKLT_EPWM.X/can.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/can.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/in-out.o: ../FLTDECKLT_EPWM.X/in-out.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/in-out.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/in-out.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/in-out.o   ../FLTDECKLT_EPWM.X/in-out.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/in-out.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/in-out.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/main.o: ../FLTDECKLT_EPWM.X/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/main.o   ../FLTDECKLT_EPWM.X/main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/mcu.o: ../FLTDECKLT_EPWM.X/mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/mcu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/mcu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/mcu.o   ../FLTDECKLT_EPWM.X/mcu.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/mcu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/mcu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1160731302/utils.o: ../FLTDECKLT_EPWM.X/utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1160731302" 
	@${RM} ${OBJECTDIR}/_ext/1160731302/utils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1160731302/utils.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1160731302/utils.o   ../FLTDECKLT_EPWM.X/utils.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1160731302/utils.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1160731302/utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/FLTDECLKLT_POT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/FLTDECLKLT_POT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/FLTDECLKLT_POT.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w  -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/FLTDECLKLT_POT.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
