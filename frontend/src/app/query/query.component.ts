import { Component, OnInit } from '@angular/core';
import { FormArray, FormBuilder } from '@angular/forms';
import { BehaviorSubject, debounceTime, interval, of } from 'rxjs';
import { concatMap, map } from 'rxjs/operators';
import { MatCheckboxChange } from '@angular/material/checkbox';
import { QueryService } from './query.service';
import { Query } from './query.model';

interface Settings {
  ignoreEmpty: boolean;
}
type SettingsId = 'ignoreEmpty';

@Component({
  selector: 'app-query',
  templateUrl: './query.component.html',
  styleUrls: ['./query.component.scss']
})
export class QueryComponent implements OnInit {

  settings: Settings = {
    ignoreEmpty: false
  };

  loadingResults$: BehaviorSubject<boolean> = new BehaviorSubject<boolean>(false);
  results$: BehaviorSubject<string[]> = new BehaviorSubject<string[]>([]);

  examplesForm = this.fb.group({
    positive: this.fb.array(['']),
    negative: this.fb.array(['']),
  });

  constructor(
    private readonly fb: FormBuilder,
    private readonly queryService: QueryService,
  ) { }

  ngOnInit(): void {
  }

  onSubmit() {
    let value: Query = this.examplesForm.getRawValue();
    if (this.settings.ignoreEmpty) {
      value.positive = value.positive.filter((example: string) => example.length > 0);
      value.negative = value.negative.filter((example: string) => example.length > 0);
    }
    // push that the results are loading
    this.loadingResults$.next(true);
    this.queryService.query(value)
      .subscribe(results => {
        this.results$.next(results);
        this.loadingResults$.next(false);
      });
  }

  get positiveExamples(): FormArray {
    return this.examplesForm.get('positive') as FormArray;
  }

  get negativeExamples(): FormArray {
    return this.examplesForm.get('negative') as FormArray;
  }

  toggleSetting(settingId: SettingsId, event: MatCheckboxChange) {
    this.settings[settingId] = event.checked;
  }
}
