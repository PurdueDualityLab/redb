import { Component, OnDestroy, OnInit } from '@angular/core';
import { FormArray, FormBuilder } from '@angular/forms';
import { BehaviorSubject, debounceTime, interval, of, Subject } from 'rxjs';
import { concatMap, map, take } from 'rxjs/operators';
import { MatCheckboxChange } from '@angular/material/checkbox';
import { QueryService } from './query.service';
import { Query } from './query.model';
import { ActivatedRoute, Router } from '@angular/router';
import { MatDialog } from '@angular/material/dialog';
import { TutorialDialogComponent } from './tutorial-dialog/tutorial-dialog.component';
import { QueryEvent, TrackingInfo } from '../tracking/tracking-info.model';
import { TrackingService } from '../tracking/tracking.service';

interface Settings {
  ignoreEmpty: boolean;
}
type SettingsId = 'ignoreEmpty';

@Component({
  selector: 'app-query',
  templateUrl: './query.component.html',
  styleUrls: ['./query.component.scss']
})
export class QueryComponent implements OnInit, OnDestroy {

  settings: Settings = {
    ignoreEmpty: false
  };

  loadingResults$: BehaviorSubject<boolean> = new BehaviorSubject<boolean>(false);
  results$: BehaviorSubject<string[]> = new BehaviorSubject<string[]>([]);
  currentTrackingInfo: TrackingInfo | null = null;

  examplesForm = this.fb.group({
    positive: this.fb.array(['']),
    negative: this.fb.array(['']),
  });

  constructor(
    private readonly fb: FormBuilder,
    private readonly queryService: QueryService,
    private readonly trackingService: TrackingService,
    private readonly navigationService: Router,
    private readonly matDialog: MatDialog,
    private readonly activatedRoute: ActivatedRoute,
  ) { }

  ngOnInit(): void {
    this.activatedRoute.queryParamMap.pipe(take(1)).subscribe(paramMap => {
      const taskId = paramMap.get('taskID');
      const participantId = paramMap.get('participantID');

      // If neither are null
      if (!(taskId == null || participantId == null)) {
        console.log(`Tracking info found: (${participantId}, ${taskId})`)
        this.currentTrackingInfo = { taskId, participantId };
      } else {
        console.log('No tracking info found');
      }
    });
  }

  ngOnDestroy(): void {
    this.loadingResults$.complete();
    this.results$.complete();
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
        if (results) {
          this.results$.next(results);
          this.loadingResults$.next(false);
        } else {
          // navigate to error page
          this.navigationService.navigate(['/error']).then();
        }
      });

    // Make a new query event
    if (this.currentTrackingInfo) {
      console.log(`Sending tracking event for participantId ${this.currentTrackingInfo.participantId}, taskId ${this.currentTrackingInfo.taskId}`);
      const event: QueryEvent = {
        taskId: this.currentTrackingInfo.taskId,
        participantId: this.currentTrackingInfo.participantId,
        positiveExamples: value.positive,
        negativeExamples: value.negative
      }
  
      this.trackingService.reportQueryEvent(event).pipe(take(1)).subscribe(() => {
        console.log("Successfully recorded query");
      });
    }
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

  showTutorialDialog(): void {
    this.matDialog.open(TutorialDialogComponent);
  }
}
