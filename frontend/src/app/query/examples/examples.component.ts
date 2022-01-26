import { Component, Input, OnInit } from '@angular/core';
import { FormArray, FormBuilder, FormControl } from '@angular/forms';

@Component({
  selector: 'app-examples',
  templateUrl: './examples.component.html',
  styleUrls: ['./examples.component.scss']
})
export class ExamplesComponent implements OnInit {

  @Input()
  label: string = '';

  @Input()
  description: string = '';

  @Input()
  examples?: FormArray;

  constructor(
    private readonly fb: FormBuilder,
  ) { }

  ngOnInit(): void {
  }

  get controls(): FormControl[] {
    return this.examples?.controls as FormControl[] || [];
  }

  public addControl() {
    this.examples?.push(this.fb.control(null));
  }

  public removeControl(idx: number) {
    this.examples?.removeAt(idx);
  }
}
