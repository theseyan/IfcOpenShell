
import { describe, expect, it, vi } from 'vitest';
import { Entity, PsetProperties } from '../src/index.js';
import type { IfcOpenShell } from '../src/index.js';

describe('PsetProperties', () => {
  it('maps JS values to native property setters', () => {
    const raw = {
      propsNew: vi.fn(() => 42),
      propsFree: vi.fn(),
      propsSetNull: vi.fn(),
      propsSetBool: vi.fn(),
      propsSetInt: vi.fn(),
      propsSetDouble: vi.fn(),
      propsSetString: vi.fn(),
      propsSetInstance: vi.fn(),
      propsSetStringList: vi.fn(),
      propsSetDoubleList: vi.fn(),
      propsSetIntList: vi.fn(),
      propsSetInstanceList: vi.fn(),
      propsSetDict: vi.fn(),
      propsSetTypedBool: vi.fn(),
      propsSetTypedDouble: vi.fn(),
      propsSetTypedInt: vi.fn(),
      propsSetTypedString: vi.fn(),
      propsSetUnitForLast: vi.fn(),
    };
    const shell = { raw: { pset: raw } } as unknown as IfcOpenShell;

    using props = new PsetProperties(shell, {
      Reference: 'A-101',
      FireRating: { value: '2HR', type: 'IfcLabel' },
      LoadBearing: true,
      Count: 3n,
      Ratio: 1.5,
      Tags: ['core', 'rated'],
      Factors: [1, 2],
      Nested: { Note: null },
    });

    expect(props.raw).toBe(42);
    expect(raw.propsSetString).toHaveBeenCalledWith(42, 'Reference', 'A-101');
    expect(raw.propsSetTypedString).toHaveBeenCalledWith(42, 'FireRating', '2HR', 'IfcLabel');
    expect(raw.propsSetBool).toHaveBeenCalledWith(42, 'LoadBearing', true);
    expect(raw.propsSetInt).toHaveBeenCalledWith(42, 'Count', 3n);
    expect(raw.propsSetDouble).toHaveBeenCalledWith(42, 'Ratio', 1.5);
    expect(raw.propsSetStringList).toHaveBeenCalledWith(42, 'Tags', ['core', 'rated']);
    expect(raw.propsSetDoubleList).toHaveBeenCalledWith(42, 'Factors', [1, 2]);
    expect(raw.propsSetNull).toHaveBeenCalledWith(42, 'Note');
    expect(raw.propsSetDict).toHaveBeenCalledWith(42, 'Nested', 42);
  });

  it('maps entity lists through parse instance lists', () => {
    const list = { destroy: vi.fn() };
    const entity = Object.create(Entity.prototype) as Entity;
    Object.defineProperty(entity, 'raw', { value: { ptr: 7 } });
    const raw = {
      propsNew: vi.fn(() => 42),
      propsFree: vi.fn(),
      propsSetInstanceList: vi.fn(),
    };
    const shell = {
      raw: {
        pset: raw,
        parse: {
          instanceListCreateFromHandles: vi.fn(() => list),
        },
      },
    } as unknown as IfcOpenShell;

    using props = new PsetProperties(shell, { Items: [entity] });

    expect(props.raw).toBe(42);
    expect(shell.raw.parse.instanceListCreateFromHandles).toHaveBeenCalledWith([entity.raw]);
    expect(raw.propsSetInstanceList).toHaveBeenCalledWith(42, 'Items', list);
    expect(list.destroy).toHaveBeenCalledOnce();
  });
});
