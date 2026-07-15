import { afterAll, beforeAll, expect, it } from 'vitest';
import { IfcFile, type Entity, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated property template, cogo, and library API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
    await shell.loadPlugin('schema', 'ifc2x3');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  it('creates and reuses typed property enumerations', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    using psetTemplate = shell.api.pset.templateAddPsetTemplate(
      file,
      'Pset_Test',
      'PSET_TYPEDRIVENOVERRIDE',
      'IfcWall',
    );
    using propTemplate = shell.api.pset.templateAddPropTemplate(
      file,
      psetTemplate,
      'Status',
      '',
      'P_ENUMERATEDVALUE',
      'IfcLabel',
    );

    shell.api.pset.templateEditPropTemplate(file, {
      propTemplate,
      attributes: { Enumerators: ['NEW', 'EXISTING'] },
    });
    using enumeration = propTemplate.get('Enumerators') as Entity;
    expect(enumeration.type).toBe('IfcPropertyEnumeration');
    const firstValues = enumeration.get('EnumerationValues') as Entity[];
    expect(firstValues.map((value) => value.type)).toEqual(['IfcLabel', 'IfcLabel']);
    expect(firstValues.map((value) => value.get(0))).toEqual(['NEW', 'EXISTING']);
    firstValues.forEach((value) => value.dispose());

    shell.api.pset.templateEditPropTemplate(file, {
      propTemplate,
      attributes: {
        PrimaryMeasureType: 'IfcInteger',
        Enumerators: [1n, 3n, 5n],
      },
    });
    using reused = propTemplate.get('Enumerators') as Entity;
    expect(reused.id).toBe(enumeration.id);
    const integerValues = reused.get('EnumerationValues') as Entity[];
    expect(integerValues.map((value) => value.type)).toEqual(['IfcInteger', 'IfcInteger', 'IfcInteger']);
    expect(integerValues.map((value) => value.get(0))).toEqual([1, 3, 5]);
    integerValues.forEach((value) => value.dispose());
  });

  it('converts bearings and rejects invalid strings', () => {
    expect(shell.api.cogo.bearing2dd('  n\t45 30 0 e  ')).toBeCloseTo(44.5, 12);
    expect(shell.api.cogo.bearing2dd('S 90 E')).toBe(0);
    expect(() => shell.api.cogo.bearing2dd('N 45x E')).toThrow('Invalid bearing string');
  });

  it('normalizes library dates for IFC4 and IFC2X3', async () => {
    const isoDateTime = '2026-07-15T12:34:56.123456+05:30';
    await using ifc4 = await IfcFile.createEmpty(shell, 'IFC4');
    using library4 = shell.api.library.addLibrary(ifc4, 'IFC4 Library');
    shell.api.library.editVersionDate(ifc4, library4, isoDateTime);
    expect(library4.get('VersionDate')).toBe(isoDateTime);

    await using ifc2x3 = await IfcFile.createEmpty(shell, 'IFC2X3');
    using library2x3 = shell.api.library.addLibrary(ifc2x3, 'IFC2X3 Library');
    shell.api.library.editVersionDate(ifc2x3, library2x3, isoDateTime);
    using calendarDate = library2x3.get('VersionDate') as Entity;
    expect(calendarDate.type).toBe('IfcCalendarDate');
    expect(calendarDate.get('DayComponent')).toBe(15);
    expect(calendarDate.get('MonthComponent')).toBe(7);
    expect(calendarDate.get('YearComponent')).toBe(2026);
  });
});
